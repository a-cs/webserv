#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>
#include <vector>

class Location {
    private:

    public:
        Location();
        ~Location();

        std::string path;
        std::vector<std::string> allowedMethods;
        std::vector<std::string> indexList;
        std::string cgi;
        bool isDirectoryEnable;
        std::string redirect;
};

#endif