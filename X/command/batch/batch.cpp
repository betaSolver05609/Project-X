#include "batch.h"
#include "../../insert-utils/insert_line.h"
#include "../../db-storage-util/db-storage-insert.h"
#include "../../common_utils/common_utils.h"
#include "../../transaction-utils/TransactionContext.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <future>
#include <thread>
#include <algorithm>

namespace
{
    // Each in-flight resolution launches a curl subprocess for the OpenAI embedding call,
    // and the API rate-limits by requests/sec — unbounded parallelism just fails louder,
    // it doesn't go faster. Cap it to a small, cheap-to-reason-about window.
    size_t batchConcurrency()
    {
        unsigned hw = std::thread::hardware_concurrency();
        return std::min<size_t>(8, std::max<size_t>(1, hw == 0 ? 4u : hw));
    }
}

void handleBatch(std::stringstream &ss, KeySpace &db)
{
    std::string what;
    ss >> what;

    if (what != "INSERT")
    {
        std::cout << "Unknown BATCH option. Use BATCH INSERT <keyspace> FILEPATH=\"path\".\n";
        return;
    }

    std::string keyspace;
    ss >> keyspace;

    std::string rest;
    std::getline(ss, rest);

    std::string filepath;
    auto pos = rest.find("FILEPATH=");
    if (pos != std::string::npos)
    {
        filepath = rest.substr(pos + 9);
        if (!filepath.empty() && filepath.front() == '"')
            filepath = filepath.substr(1, filepath.size() - 2);
    }

    if (filepath.empty())
    {
        std::cout << "FAILURE : FILEPATH not provided. Use BATCH INSERT <keyspace> FILEPATH=\"path\".\n";
        return;
    }

    if (!db.exists(keyspace))
    {
        std::cout << "FAILURE : Keyspace '" << keyspace << "' does not exist.\n";
        return;
    }

    std::ifstream file(filepath);
    if (!file.is_open())
    {
        std::cout << "FAILURE : Could not open file '" << filepath << "'.\n";
        return;
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line))
    {
        if (!line.empty())
            lines.push_back(line);
    }

    if (lines.empty())
    {
        std::cout << "No records found in file.\n";
        return;
    }

    // Wrap the whole batch in one transaction unless the caller already has one open, so a
    // mid-batch failure doesn't leave the keyspace partially populated on disk.
    bool ownsTransaction = !TransactionContext::isActive();
    if (ownsTransaction)
        TransactionContext::begin(db);

    size_t concurrency = batchConcurrency();
    int successCount = 0;
    int failureCount = 0;

    for (size_t start = 0; start < lines.size(); start += concurrency)
    {
        size_t end = std::min(lines.size(), start + concurrency);

        // Resolve (parse-or-embed) this chunk concurrently. resolveInsertLine touches no
        // shared state, so this part is safe to parallelize; it's the network-bound step.
        std::vector<std::future<ResolvedLine>> futures;
        futures.reserve(end - start);
        for (size_t i = start; i < end; i++)
            futures.push_back(std::async(std::launch::async, resolveInsertLine, lines[i]));

        // Insert sequentially in original order — KeySpace/Block mutation isn't thread-safe.
        for (size_t i = start; i < end; i++)
        {
            ResolvedLine resolved = futures[i - start].get();
            if (!resolved.success)
            {
                failureCount++;
                std::cerr << "Skipped line " << (i + 1) << ": " << resolved.error << "\n";
                continue;
            }

            Transaction t = execute_insert(db, keyspace, resolved.embedding, resolved.text);
            if (t.getStatus() == "SUCCESS")
            {
                successCount++;
                saveDb(db, keyspace);
            }
            else
            {
                failureCount++;
                std::cerr << "Skipped line " << (i + 1) << ": " << t.getMessage() << "\n";
            }
        }

        int percent = static_cast<int>((end * 100) / lines.size());
        std::cout << "\rProcessed " << end << "/" << lines.size() << " (" << percent << "%)" << std::flush;
    }
    std::cout << "\n";

    if (ownsTransaction)
    {
        if (failureCount == 0)
        {
            TransactionContext::commit(db);
            std::cout << "Batch complete: " << successCount << " inserted, " << failureCount
                      << " failed. Committed.\n";
        }
        else
        {
            TransactionContext::rollback(db);
            std::cout << "Batch complete: " << successCount << " inserted, " << failureCount
                      << " failed. Rolled back (all-or-nothing).\n";
        }
    }
    else
    {
        std::cout << "Batch complete: " << successCount << " inserted, " << failureCount
                  << " failed. (part of an open transaction — COMMIT/ROLLBACK to finalize)\n";
    }
}
