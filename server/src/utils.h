#ifndef UTILS_H
#define UTILS_H

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

inline json loadJson(const std::string &filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return json::object();
    }
    json j;
    file >> j;
    return j;
}

inline void saveJson(const std::string &filename, const json &j) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Error writing to file: " << filename << std::endl;
        return;
    }
    file << j.dump(4);
}

#endif // UTILS_H
