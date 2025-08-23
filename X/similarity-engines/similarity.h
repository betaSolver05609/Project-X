#pragma once
#include <vector>

class SimilarityEngine {
public:
    virtual ~SimilarityEngine() = default;
    virtual float compute(const std::vector<float> &a, const std::vector<float> &b) const = 0;
};
