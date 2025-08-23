#pragma once
#include "../similarity.h"
#include <vector>
#include <cmath>

class EuclideanSimilarity : public SimilarityEngine {
public:
    float compute(const std::vector<float>& a,
                  const std::vector<float>& b) const override {
        if (a.size() != b.size()) return -1e9f;
        float sum = 0.0f;
        for (size_t i = 0; i < a.size(); ++i) {
            float d = a[i] - b[i];
            sum += d * d;
        }
        float dist = std::sqrt(sum);
        return 1.0f / (1.0f + dist);
    }
};
