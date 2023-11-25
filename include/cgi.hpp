#ifndef CGI_HPP
#define CGI_HPP

#include <string>
#include <csignal>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include "request.hpp"

class Cgi
{
	private:
        std::string _fullPath;
        Request &_request;
	public:
		Cgi(std::string const &fullPath, Request &request);
		~Cgi();
        
        int exec();
};

#endif
