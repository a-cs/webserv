#include "../include/cgi.hpp"

Cgi::Cgi(std::string const &fullPath, Request &request)
    : _fullPath(fullPath), _request(request) {}

Cgi::~Cgi() {}

int Cgi::exec() {
    if (access(this->_fullPath.c_str(), R_OK) != 0)
		return 500;
    return 0;
}
