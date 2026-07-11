#pragma once
#include <string>
#include <vector>

struct ResolvedLine
{
    bool success = false;
    std::string error;
    std::vector<float> embedding;
    std::string text; // empty when the line was a raw vector rather than embedded text
};

// Trims a line and classifies it as either a raw embedding vector (every token parses
// as a complete float) or free text. Raw vectors are parsed directly; text is embedded
// via the OpenAI API. Touches no shared/database state, so it's safe to call concurrently.
ResolvedLine resolveInsertLine(const std::string &rawLine);
