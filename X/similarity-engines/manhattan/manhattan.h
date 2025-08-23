#pragma once
#include "../similarity.h"
#include <cmath>
#include <vector>
#include <stdexcept>

class ManhattanSimilarity : public SimilarityEngine {
public:
    float compute(const std::vector<float>& a, const std::vector<float>& b) const override {
        if (a.size() != b.size())
            throw std::invalid_argument("Vectors must be same dimension");

        float dist = 0.0f;
        for (size_t i = 0; i < a.size(); i++)
            dist += std::fabs(a[i] - b[i]);

        // Normalize to similarity: higher is better → similarity = 1 / (1 + distance)
        return 1.0f / (1.0f + dist);
    }
};
