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

#define MAX_EVENTS 5

class Server {
	private:
		int sock;
		int port;
		int epollFd;
		struct epoll_event event;
		struct epoll_event events[MAX_EVENTS];

	public:
		Server();
		~Server();
		int	create();
		int start();

};

#endif