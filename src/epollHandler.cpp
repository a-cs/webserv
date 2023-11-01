#include "../include/epollHandler.hpp"

EpollHandler::EpollHandler() {
	epollFd = epoll_create(1);
	events = new epoll_event[MAX_EVENTS];
	memset(events, 0, sizeof(epoll_event) * MAX_EVENTS);
}

EpollHandler::~EpollHandler() {
	delete[] events;
	close(epollFd);
}
