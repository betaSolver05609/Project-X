#pragma once
#include "similarity.h"
#include "cosine/cosine.h"
#include "dot/dot.h"
#include "euclidean/euclidean.h"
#include "manhattan/manhattan.h"
#include "chebyshev/chebyshev.h"
#include "jaccard/jaccard.h"
#include "mahalanobis/mahalanobis.h"
#include "hamming/hamming.h"
#include "js-divergence/jsdivergance.h"
#include "minkowski/minkowski.h"
#include "dot/dot.h"
#include <iostream>
#include <memory>
#include <string>

inline std::unique_ptr<SimilarityEngine> createSimilarityEngine(std::string metric)
{
    // lowercase normalize
    std::transform(metric.begin(), metric.end(), metric.begin(),
                   [](unsigned char c){ return std::tolower(c); });

    if (metric == "cosine") return std::make_unique<CosineSimilarity>();
    else if (metric == "euclidean") return std::make_unique<EuclideanSimilarity>();
    else if (metric == "manhattan") return std::make_unique<ManhattanSimilarity>();
    else if (metric == "chebyshev") return std::make_unique<ChebyshevSimilarity>();
    else if (metric == "jaccard") return std::make_unique<JaccardSimilarity>();
    else if (metric == "mahalanobis") return std::make_unique<MahalanobisSimilarity>();
    else if (metric == "hamming") return std::make_unique<HammingSimilarity>();
    else if (metric == "jsd") return std::make_unique<JensenShannonSimilarity>();
    else if (metric == "minkowski") return std::make_unique<MinkowskiSimilarity>();
    else if (metric == "dot") return std::make_unique<DotProductSimilarity>();
    else throw std::invalid_argument("Unknown similarity metric: " + metric);
}

