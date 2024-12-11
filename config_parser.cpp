#include "config_parser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

Config parseConfigFile(const std::string &filename) {
    Config config;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open config file. Ensure it exists at: " << filename << "\n";
        return {25, 16, 50};
    }

    std::string line;
    std::vector<int> values;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        int value;
        iss >> value;
        values.push_back(value);
    }

    file.close();

    if (values.size() >= 3) {
        config.cols = values[0];
        config.rows = values[1];
        config.mines = values[2];
    } else {
        std::cerr << "Error: Invalid config file format. Using default values.\n";
        config = {25, 16, 50};
    }

    return config;
}