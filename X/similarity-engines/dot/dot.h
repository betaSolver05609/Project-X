#pragma once
#include "../similarity.h"

class DotProductSimilarity : public SimilarityEngine {
public:
    float compute(const std::vector<float> &a, const std::vector<float> &b) const override {
        float dot = 0.0;
        for (size_t i = 0; i < a.size(); i++)
            dot += a[i] * b[i];
        return dot;
    }
};
