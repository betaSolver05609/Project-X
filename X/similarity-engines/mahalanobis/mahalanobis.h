#pragma once
#include "../similarity.h"
#include <vector>
#include <cmath>
#include <stdexcept>
#include <algorithm>

// ---- Free function, no override here ----
inline std::vector<std::vector<float>> computeCovariance(const std::vector<std::vector<float>>& data) {
    if (data.empty())
        throw std::invalid_argument("Dataset is empty");

    size_t n = data.size();
    size_t dim = data[0].size();

    // Mean
    std::vector<float> mean(dim, 0.0f);
    for (const auto& row : data) {
        if (row.size() != dim) throw std::invalid_argument("Inconsistent dimensions");
        for (size_t i = 0; i < dim; i++)
            mean[i] += row[i];
    }
    for (size_t i = 0; i < dim; i++) mean[i] /= n;

    // Covariance matrix
    std::vector<std::vector<float>> cov(dim, std::vector<float>(dim, 0.0f));
    for (const auto& row : data) {
        for (size_t i = 0; i < dim; i++) {
            for (size_t j = 0; j < dim; j++) {
                cov[i][j] += (row[i] - mean[i]) * (row[j] - mean[j]);
            }
        }
    }
    for (size_t i = 0; i < dim; i++) {
        for (size_t j = 0; j < dim; j++) {
            cov[i][j] /= (n - 1);
        }
    }

    return cov;
}

// ---- Mahalanobis similarity class ----
class MahalanobisSimilarity : public SimilarityEngine {
private:
    std::vector<std::vector<float>> covInv; // cached covariance inverse

    // Matrix inversion (Gaussian elimination)
    inline static std::vector<std::vector<float>> invertMatrix(const std::vector<std::vector<float>>& m) {
        int n = m.size();
        std::vector<std::vector<float>> a = m;
        std::vector<std::vector<float>> inv(n, std::vector<float>(n, 0.0f));

        // Initialize identity
        for (int i = 0; i < n; i++) inv[i][i] = 1.0f;

        // Forward elimination
        for (int i = 0; i < n; i++) {
            float diag = a[i][i];
            if (std::fabs(diag) < 1e-9f)
                throw std::invalid_argument("Singular matrix in Mahalanobis");

            for (int j = 0; j < n; j++) {
                a[i][j] /= diag;
                inv[i][j] /= diag;
            }
            for (int k = 0; k < n; k++) {
                if (k == i) continue;
                float factor = a[k][i];
                for (int j = 0; j < n; j++) {
                    a[k][j] -= factor * a[i][j];
                    inv[k][j] -= factor * inv[i][j];
                }
            }
        }
        return inv;
    }

    // Compute Mahalanobis distance
    inline static float computeMahalanobis(const std::vector<float>& a,
                                           const std::vector<float>& b,
                                           const std::vector<std::vector<float>>& covInv) {
        if (a.size() != b.size())
            throw std::invalid_argument("Dimension mismatch in Mahalanobis");

        int dim = a.size();
        std::vector<float> diff(dim);
        for (int i = 0; i < dim; i++) diff[i] = a[i] - b[i];

        // v^T * covInv * v
        std::vector<float> temp(dim, 0.0f);
        for (int i = 0; i < dim; i++) {
            for (int j = 0; j < dim; j++) {
                temp[i] += diff[j] * covInv[j][i];
            }
        }

        float dist = 0.0f;
        for (int i = 0; i < dim; i++) {
            dist += temp[i] * diff[i];
        }

        return std::sqrt(dist);
    }

public:
    MahalanobisSimilarity() = default;

    void setDataset(const std::vector<std::vector<float>>& dataset) {
        auto cov = computeCovariance(dataset);
        covInv = invertMatrix(cov);
    }

    float compute(const std::vector<float>& a, const std::vector<float>& b) const override {
        if (covInv.empty())
            throw std::runtime_error("Covariance matrix not initialized. Call setDataset first.");
        return computeMahalanobis(a, b, covInv);
    }

    // helper (not in interface, so no override)
    float computeWithDataset(const std::vector<float>& a,
                             const std::vector<float>& b,
                             const std::vector<std::vector<float>>& dataset) const {
        auto cov = computeCovariance(dataset);
        auto covInvTemp = invertMatrix(cov);
        return computeMahalanobis(a, b, covInvTemp);
    }
};
