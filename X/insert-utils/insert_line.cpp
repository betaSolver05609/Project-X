#include "insert_line.h"
#include "../common_utils/common_utils.h"
#include "../embedding-utils/embedding.h"
#include <sstream>

namespace
{
    std::string trim(const std::string &s)
    {
        size_t start = s.find_first_not_of(" \t\r\n");
        if (start == std::string::npos)
            return "";
        size_t end = s.find_last_not_of(" \t\r\n");
        return s.substr(start, end - start + 1);
    }

    // True only if every whitespace/comma/bracket-separated token is a complete number,
    // i.e. this looks like a hand-typed embedding vector rather than natural-language text.
    bool looksLikeVector(const std::string &s)
    {
        std::string cleaned;
        cleaned.reserve(s.size());
        for (char c : s)
            cleaned += (c == ',' || c == '[' || c == ']' || c == '(' || c == ')') ? ' ' : c;

        std::istringstream iss(cleaned);
        std::string token;
        bool sawToken = false;
        while (iss >> token)
        {
            sawToken = true;
            try
            {
                size_t consumed = 0;
                std::stof(token, &consumed);
                if (consumed != token.size())
                    return false;
            }
            catch (...)
            {
                return false;
            }
        }
        return sawToken;
    }
}

ResolvedLine resolveInsertLine(const std::string &rawLine)
{
    ResolvedLine result;
    std::string trimmed = trim(rawLine);

    if (trimmed.empty())
    {
        result.error = "No data provided to insert.";
        return result;
    }

    if (looksLikeVector(trimmed))
    {
        result.embedding = parseVector(trimmed);
        result.success = true;
        return result;
    }

    EmbeddingResult embedded = getEmbedding(trimmed);
    if (!embedded.success)
    {
        result.error = embedded.error;
        return result;
    }

    result.embedding = embedded.embedding;
    result.text = trimmed;
    result.success = true;
    return result;
}
