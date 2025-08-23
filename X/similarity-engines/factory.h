#pragma once
#include "similarity.h"
#include "cosine/cosine.h"
#include "euclidean/euclidean.h"
#include "dot/dot.h"
#include <iostream>
#include <memory>
#include <string>

inline std::unique_ptr<SimilarityEngine> createSimilarityEngine(std::string metric) {
    // lowercase normalize
    std::transform(metric.begin(), metric.end(), metric.begin(),
                   [](unsigned char c){ return std::tolower(c); });

    std::cout << "[DEBUG] Using metric: " << metric << "\n";

    if (metric == "cosine")
        return std::make_unique<CosineSimilarity>();
    else if (metric == "euclidean")
        return std::make_unique<EuclideanSimilarity>();
    else if (metric == "dot")
        return std::make_unique<DotProductSimilarity>();
    else
        return std::make_unique<CosineSimilarity>(); // default fallback
}

