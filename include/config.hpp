#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>
#include "location.hpp"
#include "error.hpp"

class Config {
    private:

    public:
        Config();
        ~Config();

        int port;
        std::string root;
        std::vector<std::string> serverNamesList;
        long double bodySizeLimit;
        std::vector<Location> locationList;
        Error error;
};

#endif
