#pragma once
#include <vector>

class SimilarityEngine {
public:
    virtual ~SimilarityEngine() = default;

    // Non-const; derived classes must match this signature exactly
    virtual float compute(const std::vector<float>& a,
                          const std::vector<float>& b) const= 0;

    // For metrics that need dataset context (e.g., Mahalanobis).
    // Default falls back to the simple compute.
    virtual float computeWithDataset(const std::vector<float>& a,
                                     const std::vector<float>& b,
                                     const std::vector<std::vector<float>>& dataset) const {
        return compute(a, b);
    }
};
