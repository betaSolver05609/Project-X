#pragma once
#include "../similarity.h"
#include <cmath>

class CosineSimilarity : public SimilarityEngine {
public:
    float compute(const std::vector<float> &a, const std::vector<float> &b) const override {
        float dot = 0.0, normA = 0.0, normB = 0.0;
        for (size_t i = 0; i < a.size(); i++) {
            dot += a[i] * b[i];
            normA += a[i] * a[i];
            normB += b[i] * b[i];
        }
        if (normA == 0 || normB == 0) return 0.0f;
        return dot / (std::sqrt(normA) * std::sqrt(normB));
    }
};
