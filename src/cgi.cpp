#include "../include/cgi.hpp"

Cgi::Cgi(std::string const &fullPath, Request &request)
    : _fullPath(fullPath), _request(request) {}

Cgi::~Cgi() {}

int Cgi::exec() {
    
    return 0;
}
