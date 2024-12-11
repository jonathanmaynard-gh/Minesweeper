#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <string>

struct Config {
    int cols;
    int rows;
    int mines;
};

Config parseConfigFile(const std::string &filename);

#endif
