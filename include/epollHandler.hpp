#ifndef EPOLL_HANDLER_HPP
# define EPOLL_HANDLER_HPP

#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <unistd.h>
#include <strings.h>
#include <csignal>
#include <string.h>

#define MAX_EVENTS 5

class EpollHandler {
	private:

	public:
        int epollFd;
		struct epoll_event *events;

		EpollHandler();
		~EpollHandler();
};

#endif
