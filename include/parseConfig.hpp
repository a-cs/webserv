#ifndef PARSE_CONFIG_HPP
#define PARSE_CONFIG_HPP

#include <string>
#include <vector>
#include "config.hpp"

class ParseConfig {
    public:
        ParseConfig(std::string file);
        ~ParseConfig();

        std::vector<Config> configList;
};

#endif
