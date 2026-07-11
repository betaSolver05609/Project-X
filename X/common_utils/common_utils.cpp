#include "common_utils.h"
#include "../transaction-utils/TransactionContext.h"
#include <sstream>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <cstdint>
#include <cstring>

std::vector<float> parseVector(const std::string &input)
{
    std::vector<float> vec;
    std::stringstream ss(input);
    char c;
    float num;
    while (ss >> c)
    {
        if ((c >= '0' && c <= '9') || c == '-' || c == '.')
        {
            ss.putback(c);
            ss >> num;
            vec.push_back(num);
        }
    }
    return vec;
}

namespace
{
    constexpr char MAGIC[4] = {'P', 'X', 'V', 'B'};
    constexpr uint32_t FORMAT_VERSION = 2;

    void writeKeyspaceFile(const std::string &name, const std::string &metric,
                            const std::vector<Block::Record> &records)
    {
        std::filesystem::create_directory("data");

        std::string finalPath = "data/" + name + ".bin";
        std::string tmpPath = finalPath + ".tmp";

        {
            std::ofstream file(tmpPath, std::ios::binary | std::ios::trunc);
            if (!file.is_open())
            {
                std::cerr << "Failed to open temp file for keyspace '" << name << "'\n";
                return;
            }

            file.write(MAGIC, sizeof(MAGIC));
            file.write(reinterpret_cast<const char *>(&FORMAT_VERSION), sizeof(FORMAT_VERSION));

            uint32_t metricLen = static_cast<uint32_t>(metric.size());
            file.write(reinterpret_cast<const char *>(&metricLen), sizeof(metricLen));
            file.write(metric.data(), metricLen);

            uint64_t recordCount = static_cast<uint64_t>(records.size());
            file.write(reinterpret_cast<const char *>(&recordCount), sizeof(recordCount));

            for (auto &rec : records)
            {
                uint32_t recLen = static_cast<uint32_t>(rec.embedding.size());
                file.write(reinterpret_cast<const char *>(&recLen), sizeof(recLen));
                if (recLen > 0)
                    file.write(reinterpret_cast<const char *>(rec.embedding.data()), recLen * sizeof(float));

                uint32_t textLen = static_cast<uint32_t>(rec.text.size());
                file.write(reinterpret_cast<const char *>(&textLen), sizeof(textLen));
                if (textLen > 0)
                    file.write(rec.text.data(), textLen);
            }

            file.flush();
            if (!file.good())
            {
                std::cerr << "Failed to write temp file for keyspace '" << name << "'\n";
                return;
            }
        }

        std::error_code ec;
        std::filesystem::rename(tmpPath, finalPath, ec);
        if (ec)
        {
            std::cerr << "Failed to persist keyspace '" << name << "': " << ec.message() << "\n";
        }
    }

    bool readKeyspaceFile(const std::filesystem::path &path, std::string &metricOut,
                           std::vector<Block::Record> &recordsOut)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
            return false;

        char magic[4];
        file.read(magic, sizeof(magic));
        if (!file || std::memcmp(magic, MAGIC, sizeof(MAGIC)) != 0)
            return false;

        uint32_t version = 0;
        file.read(reinterpret_cast<char *>(&version), sizeof(version));
        if (!file || version != FORMAT_VERSION)
            return false;

        uint32_t metricLen = 0;
        file.read(reinterpret_cast<char *>(&metricLen), sizeof(metricLen));
        if (!file)
            return false;

        metricOut.resize(metricLen);
        if (metricLen > 0)
            file.read(&metricOut[0], metricLen);
        if (!file)
            return false;

        uint64_t recordCount = 0;
        file.read(reinterpret_cast<char *>(&recordCount), sizeof(recordCount));
        if (!file)
            return false;

        recordsOut.clear();
        recordsOut.reserve(static_cast<size_t>(recordCount));

        for (uint64_t i = 0; i < recordCount; i++)
        {
            uint32_t recLen = 0;
            file.read(reinterpret_cast<char *>(&recLen), sizeof(recLen));
            if (!file)
                return false;

            std::vector<float> vec(recLen);
            if (recLen > 0)
            {
                file.read(reinterpret_cast<char *>(vec.data()), recLen * sizeof(float));
                if (!file)
                    return false;
            }

            uint32_t textLen = 0;
            file.read(reinterpret_cast<char *>(&textLen), sizeof(textLen));
            if (!file)
                return false;

            std::string text(textLen, '\0');
            if (textLen > 0)
            {
                file.read(&text[0], textLen);
                if (!file)
                    return false;
            }

            recordsOut.push_back({std::move(vec), std::move(text)});
        }

        return true;
    }
}

void saveDb(const KeySpace &db)
{
    for (auto &name : db.listKeyspaces())
    {
        writeKeyspaceFile(name, db.getMetric(name), db.getRecordsFull(name));
    }
}

void saveDb(const KeySpace &db, const std::string &keyspaceName)
{
    if (!db.exists(keyspaceName))
        return;

    if (TransactionContext::isActive())
    {
        TransactionContext::markDirty(keyspaceName);
        return;
    }

    writeKeyspaceFile(keyspaceName, db.getMetric(keyspaceName), db.getRecordsFull(keyspaceName));
}

void loadDb(KeySpace &db)
{
    std::filesystem::create_directory("data");

    for (auto &entry : std::filesystem::directory_iterator("data"))
    {
        if (entry.path().extension() != ".bin")
            continue;

        std::string name = entry.path().stem().string();
        std::string metric;
        std::vector<Block::Record> records;

        if (!readKeyspaceFile(entry.path(), metric, records))
        {
            std::cerr << "Warning: failed to load keyspace file '" << entry.path().string()
                       << "', skipping.\n";
            continue;
        }

        db.createKeySpace(name, metric.empty() ? "cosine" : metric);
        for (auto &rec : records)
            db.insertIntoBlock(name, rec.embedding, rec.text);
    }
}
