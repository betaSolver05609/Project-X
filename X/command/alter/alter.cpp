#include "alter.h"
#include "../../common_utils/common_utils.h"
#include <iostream>
#include <algorithm>

bool isValidMetric(const std::string &metric)
{
    std::string m = metric;
    std::transform(m.begin(), m.end(), m.begin(),
                   [](unsigned char c)
                   { return std::tolower(c); });

    return (m == "cosine" ||
            m == "euclidean" ||
            m == "manhattan" ||
            m == "chebyshev" ||
            m == "jaccard" ||
            m == "mahalanobis" ||
            m == "hamming" ||
            m == "jsd" ||
            m == "minkowski" ||
            m == "dot");
}

void handleAlter(std::stringstream &ss, KeySpace &db)
{
    std::string what;
    ss >> what;

    if (what == "KEYSPACE")
    {
        std::string name, setWord, metricWord, metricType;
        ss >> name >> setWord >> metricWord >> metricType;

        if (setWord == "SET" && metricWord == "METRIC")
        {
            if (!isValidMetric(metricType))
            {
                std::cout << "Invalid metric type '" << metricType << "'.\n"
                          << "Supported metrics: COSINE, EUCLIDEAN, MANHATTAN, "
                          << "CHEBYSHEV, JACCARD, MAHALANOBIS, HAMMING, JSD, "
                          << "MINKOWSKI, DOT.\n";
                return;
            }
            if (db.alterMetric(name, metricType))
            {
                std::cout << "Keyspace '" << name
                          << "' metric changed to '" << metricType << "'.\n";
                saveDb(db);
            }
            else
            {
                std::cout << "Keyspace '" << name << "' does not exist.\n";
            }
        }
        else
        {
            std::cout << "Invalid ALTER syntax. Usage: ALTER KEYSPACE <name> SET METRIC <type>\n";
        }
    }
    else
    {
        std::cout << "Unknown ALTER option.\n";
    }
}
