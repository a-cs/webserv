#ifndef CGI_HPP
#define CGI_HPP

#include <string>
#include <csignal>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include "server.hpp"

class Cgi
{
	private:
		Request request;
        std::string fullPath;
		std::string	port;
        Server &server;
		int		tempFd;
		char	**args;
		char	**envp;

		char	**createArrayOfStrings(std::vector<std::string> const &vector) const;
		void	destroyArrayOfStrings(char **array) const;
		void	prepareCGI();
		void	executeCgi();

	public:
		Cgi(std::string const &fullPath, Server &server, Request &request);
		~Cgi();
        
        std::string	exec();
};

#endif
