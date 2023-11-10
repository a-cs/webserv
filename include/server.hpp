#ifndef SERVER_HPP
# define SERVER_HPP

#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <unistd.h>
#include <strings.h>
#include <csignal>

class Server {
	private:
		int sock;
		int port;

	public:
		Server();
		~Server();
		int			getSock();
		int			create();
		std::string	getRequestData(int fd);
};

#endif
