#pragma once
#include "../similarity.h"
#include <vector>

class DotProductSimilarity : public SimilarityEngine {
public:
    float compute(const std::vector<float>& a,
                  const std::vector<float>& b) const override {
        if (a.size() != b.size()) return -1e9f;
        float dot = 0.0f;
        for (size_t i = 0; i < a.size(); ++i) dot += a[i] * b[i];
        return dot;
    }
};
