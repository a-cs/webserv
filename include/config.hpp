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
        void clear();

        int port; //todo: transformar em lista
        std::string root;
        std::vector<std::string> serverNamesList;
        long double bodySizeLimit;
        std::vector<Location> locationList;
        std::vector<std::string> errorPageList;
        Error error; // todo: adicionar lista de erros
};

#endif
