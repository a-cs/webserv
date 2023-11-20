#ifndef PARSE_CONFIG_HPP
#define PARSE_CONFIG_HPP

#include <string>
#include <vector>
#include "config.hpp"
#include "error.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include "utils.hpp"
#include <list>
#include <cstdlib>
#include <algorithm>

class ParseConfig {
    private:
        std::ifstream fd;

        std::string readFile(std::string file);
        std::list<std::string> parseFile(std::string fileContent);
        void getConfig(std::list<std::string> lines);
        void addConfigProperties(std::string line, Config *config);
        void addLocationProperties(std::string line, Location *location);

    public:
        ParseConfig(std::string file);
        ~ParseConfig();
        Error error;

        std::vector<Config> configList;
};

#endif
