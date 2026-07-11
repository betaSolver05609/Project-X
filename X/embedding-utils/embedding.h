#pragma once
#include <string>
#include <vector>

struct EmbeddingResult
{
    bool success = false;
    std::string error;
    std::vector<float> embedding;
};

// Converts text into an embedding vector via the OpenAI embeddings API.
// Requires the OPENAI_API_KEY environment variable to be set.
EmbeddingResult getEmbedding(const std::string &text, const std::string &model = "text-embedding-3-small");
