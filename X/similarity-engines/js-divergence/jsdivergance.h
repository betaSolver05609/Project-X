#pragma once
#include "../similarity.h"
#include <vector>
#include <cmath>
#include <stdexcept>

class JensenShannonSimilarity : public SimilarityEngine {
private:
    static float klDivergence(const std::vector<float>& p, const std::vector<float>& q) {
        float div = 0.0f;
        for (size_t i = 0; i < p.size(); i++) {
            if (p[i] > 0 && q[i] > 0) {
                div += p[i] * log(p[i] / q[i]);
            }
        }
        return div;
    }

public:
    float compute(const std::vector<float>& a,
                  const std::vector<float>& b) const override {
        if (a.size() != b.size())
            throw std::invalid_argument("Dimension mismatch");

        // normalize to probabilities
        float sumA = 0, sumB = 0;
        for (float v : a) sumA += v;
        for (float v : b) sumB += v;
        if (sumA == 0 || sumB == 0)
            throw std::invalid_argument("Zero vector in JSD");

        std::vector<float> p(a.size()), q(b.size()), m(a.size());
        for (size_t i = 0; i < a.size(); i++) {
            p[i] = a[i] / sumA;
            q[i] = b[i] / sumB;
            m[i] = 0.5f * (p[i] + q[i]);
        }

        float jsd = 0.5f * klDivergence(p, m) + 0.5f * klDivergence(q, m);
        return 1.0f - sqrt(jsd);  // similarity score
    }
};
