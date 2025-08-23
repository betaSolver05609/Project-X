#include "common_utils.h"
#include <sstream>
#include <fstream>
#include <filesystem>

std::vector<float> parseVector(const std::string &input)
{
    std::vector<float> vec;
    std::stringstream ss(input);
    char c;
    float num;
    while (ss >> c)
    {
        if ((c >= '0' && c <= '9') || c == '-' || c == '.')
        {
            ss.putback(c);
            ss >> num;
            vec.push_back(num);
        }
    }
    return vec;
}

void saveDb(const KeySpace &db)
{
    std::filesystem::create_directory("data");

    for (auto &name : db.listKeyspaces())
    {
        Json::Value root = db.toJson(name);
        std::ofstream file("data/" + name + ".json");
        if (file.is_open())
        {
            Json::StreamWriterBuilder writer;
            writer["indentation"] = "  ";
            file << Json::writeString(writer, root);
        }
    }
}

void loadDb(KeySpace &db)
{
    std::filesystem::create_directory("data");

    for (auto &entry : std::filesystem::directory_iterator("data"))
    {
        if (entry.path().extension() == ".json")
        {
            std::ifstream file(entry.path());
            if (file.is_open())
            {
                Json::Value root;
                file >> root;
                std::string name = entry.path().stem().string();
                db.fromJson(name, root);
            }
        }
    }
}
