#include <iostream>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/reader.h>
#include <memory>

using namespace std;

std::string parse_raw_json(const std::string raw_json) {
    Json::Value roots {};
    Json::CharReaderBuilder builder{};
    std::string error {};

    auto reader=std::unique_ptr<Json::CharReader>(builder.newCharReader());

    bool parseSet=reader->parse(raw_json.c_str(), raw_json.c_str()+raw_json.length(), &roots, &error);

    if(parseSet) {
        return roots.asString();
    }
    else {
        std::cout<<"Something went wrong with processing Json"<<endl;
        std::cout<<error<<endl;
        return "Something went wrong with processing Json";
    }
}