#pragma once
#include "../similarity.h"
#include <cmath>

class EuclideanSimilarity : public SimilarityEngine {
public:
    float compute(const std::vector<float> &a, const std::vector<float> &b) const override {
        float sum = 0.0;
        for (size_t i = 0; i < a.size(); i++)
            sum += (a[i] - b[i]) * (a[i] - b[i]);
        return 1.0f / (1.0f + std::sqrt(sum));
    }
};
