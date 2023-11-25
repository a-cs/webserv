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
#include "server.hpp"

class Cgi
{
	private:
        std::string fullPath;
		std::string	port;
        Server &server;
	public:
		Cgi(std::string const &fullPath, Server &server);
		~Cgi();
        
        int exec();
};

#endif
