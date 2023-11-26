#ifndef SERVER_HPP
#define SERVER_HPP

#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <unistd.h>
#include <strings.h>
#include <csignal>
#include "request.hpp"
#include "response.hpp"
#include "config.hpp"
#include "cgi.hpp"

class Server {
	private:
		int	sock;
		int	port;

	public:
		Config	config;

		Server();
		~Server();

		int			create();
		int			getSock();
		void		handleMultipart(Request *request);
		void		handleRequest(Request *request, Response *response);
		std::string	getRequestData(int fd);
};

#endif
