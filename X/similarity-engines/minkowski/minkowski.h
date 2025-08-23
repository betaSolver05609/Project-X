#pragma once
#include "../similarity.h"
#include <vector>
#include <cmath>
#include <stdexcept>

class MinkowskiSimilarity : public SimilarityEngine {
private:
    float p; // order

public:
    explicit MinkowskiSimilarity(float order = 3.0f) : p(order) {
        if (p <= 0) throw std::invalid_argument("p must be > 0");
    }

    float compute(const std::vector<float>& a,
                  const std::vector<float>& b) const override {
        if (a.size() != b.size())
            throw std::invalid_argument("Dimension mismatch");

        float sum = 0.0f;
        for (size_t i = 0; i < a.size(); i++) {
            sum += std::pow(std::fabs(a[i] - b[i]), p);
        }
        float dist = std::pow(sum, 1.0f / p);
        return 1.0f / (1.0f + dist); // similarity
    }
};
