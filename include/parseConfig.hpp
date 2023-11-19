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

class ParseConfig {
    private:
        std::ifstream fd;

        std::string readFile(std::string file);
        std::list<std::string> parseFile(std::string fileContent);
        bool isServerLine(std::string line);
        bool isLocationLine(std::string line);
        bool isClosedBrackets(std::string line);
        bool canRead(bool onServer, bool onLocation, std::string line);
        void getConfig(std::list<std::string> lines);

    public:
        ParseConfig(std::string file);
        ~ParseConfig();
        Error error;

        std::vector<Config> configList;
};

#endif
