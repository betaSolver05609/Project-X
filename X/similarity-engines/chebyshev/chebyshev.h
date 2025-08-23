#pragma once
#include "../similarity.h"
#include <cmath>
#include <vector>
#include <stdexcept>

class ChebyshevSimilarity : public SimilarityEngine {
public:
    float compute(const std::vector<float>& a, const std::vector<float>& b) const override {
        if (a.size() != b.size())
            throw std::invalid_argument("Vectors must be same dimension");

        float maxDiff = 0.0f;
        for (size_t i = 0; i < a.size(); i++)
            maxDiff = std::max(maxDiff, std::fabs(a[i] - b[i]));

        return 1.0f / (1.0f + maxDiff);
    }
};
