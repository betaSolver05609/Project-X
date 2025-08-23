#pragma once
#include "../similarity.h"
#include <unordered_set>
#include <vector>
#include <stdexcept>

class JaccardSimilarity : public SimilarityEngine {
public:
    float compute(const std::vector<float>& a, const std::vector<float>& b) const override {
        if (a.empty() && b.empty())
            return 1.0f;
        if (a.empty() || b.empty())
            return 0.0f;

        std::unordered_set<int> setA(a.begin(), a.end());
        std::unordered_set<int> setB(b.begin(), b.end());

        int intersection = 0;
        for (const auto& x : setA)
            if (setB.find(x) != setB.end())
                intersection++;

        int uni = setA.size() + setB.size() - intersection;

        return uni == 0 ? 0.0f : static_cast<float>(intersection) / uni;
    }
};
