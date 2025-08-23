#pragma once
#include "../similarity.h"
#include <vector>
#include <stdexcept>

class HammingSimilarity : public SimilarityEngine {
public:
    float compute(const std::vector<float>& a,
                  const std::vector<float>& b) const override {
        if (a.size() != b.size())
            throw std::invalid_argument("Dimension mismatch");

        int diff = 0;
        for (size_t i = 0; i < a.size(); i++) {
            if (a[i] != b[i]) diff++;
        }
        return 1.0f - static_cast<float>(diff) / a.size(); 
        // returns similarity (1 - normalized distance)
    }
};
