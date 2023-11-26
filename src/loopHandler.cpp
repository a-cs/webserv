#include "../include/server.hpp"
#include "../include/epollHandler.hpp"
#include "../include/request.hpp"
#include "../include/config.hpp"
#include <sstream>
#include <fcntl.h>

typedef struct {
	std::string type;
	int fd;
	void *ptr;
	Request req;
} connection_t;

bool isRunning(bool status) {
	static bool running = true;

	if (!status)
		running = false;
	return running;
}

void stop(int sig) {
	if (sig == SIGINT || sig == SIGQUIT) {
		std::cerr << "parou\n";
		isRunning(false);
	}
}

bool setNonBlocking(int fd) {
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		return false;
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
		std::cerr << "nonBlocking";
		return false;
	}
	return true;
}

int start(EpollHandler *epollHandler) {
	std::signal(SIGINT, stop);
	std::signal(SIGQUIT, stop);

	while (isRunning(true)) {
		int numEvents = epoll_wait(epollHandler->epollFd, epollHandler->events, MAX_EVENTS, 0);

		if (numEvents == -1) {
			std::cerr << "epoll_wait";
			return EXIT_FAILURE;
		}

		for (int i = 0; i < numEvents; i++) {
			std::string requestData;
			connection_t *connection = (connection_t *)(epollHandler->events[i].data.ptr);
			Server *server = (Server *)(connection->ptr);
			if (connection->type == "new connection") {
				struct sockaddr_in clientAddr;
				socklen_t clientAddrLen = sizeof(clientAddr);
				int clientSocket = accept(server->getSock(), (struct sockaddr *)&clientAddr, &clientAddrLen);

				if (clientSocket == -1)
					std::cerr << "\n Error accept";
				else {
					setNonBlocking(clientSocket);
					connection_t *newConnection = new connection_t;
					Request req;
					newConnection->type = "connected";
					newConnection->ptr = server;
					newConnection->fd = clientSocket;
					newConnection->req = req;
					struct epoll_event event;
					event.events = EPOLLIN;
					event.data.ptr = newConnection;
					epoll_ctl(epollHandler->epollFd, EPOLL_CTL_ADD, clientSocket, &event);
				}
				continue;
			}

			if (epollHandler->events[i].events & (EPOLLRDHUP | EPOLLHUP)) {
				epoll_ctl(epollHandler->epollFd, EPOLL_CTL_DEL, connection->fd, 0);
				close(connection->fd);
				delete connection;
				continue;
			}

			if (epollHandler->events[i].events & EPOLLIN) {
				requestData = server->getRequestData(connection->fd);
				if (!requestData.size()) {
					epoll_ctl(epollHandler->epollFd, EPOLL_CTL_DEL, connection->fd, 0);
					close(connection->fd);
					delete connection;
					continue;
				}

				connection->req.parse(requestData, &server->config);

				if (connection->req.isParsed()) {
					connection->type = "response";
					struct epoll_event event;
					event.events = EPOLLOUT;
					event.data.ptr = connection;
					epoll_ctl(epollHandler->epollFd, EPOLL_CTL_MOD, connection->fd, &event);
				}
				continue;
			}
			if (epollHandler->events[i].events & EPOLLOUT) {
				Response res;
				server->handleRequest(&connection->req, &res);
				std::string message = res.getMessage();
				write(connection->fd, message.c_str(), message.size());
				epoll_ctl(epollHandler->epollFd, EPOLL_CTL_DEL, connection->fd, 0);
				close(connection->fd);
				delete connection;
				continue;
			}
		}
	}
	return EXIT_SUCCESS;
}

int run(std::vector<Config> configList) {
	Server servers[configList.size()];
	connection_t connections[configList.size()];
	EpollHandler epollHandler;

	if (epollHandler.epollFd == -1) {
		std::cerr << "Failed to create epoll";
		return EXIT_FAILURE;
	}

	for (size_t i = 0; i < configList.size(); i++) {
		servers[i].config = configList[i];
		if (servers[i].create() != 0)
			return EXIT_FAILURE;
		connection_t *connection = &connections[i];
		connection->type = "new connection";
		connection->ptr = &servers[i];
		struct epoll_event event;
		event.events = EPOLLIN | EPOLLOUT;
		event.data.fd = servers[i].getSock();
		event.data.ptr = connection;
		if (epoll_ctl(epollHandler.epollFd, EPOLL_CTL_ADD, servers[i].getSock(), &event) == -1) {
			std::cerr << "Failed to add server socket to epoll";
			return EXIT_FAILURE;
		}
	}

	start(&epollHandler);
	return 0;
}

void	printInitialFrase(void) {
	std::cout << "\e[38;2;0;186;188m────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────\n\
─██████──────────██████─██████████████─██████████████───██████████████─██████████████─████████████████───██████──██████─\n\
─██░░██──────────██░░██─██░░░░░░░░░░██─██░░░░░░░░░░██───██░░░░░░░░░░██─██░░░░░░░░░░██─██░░░░░░░░░░░░██───██░░██──██░░██─\n\
─██░░██──────────██░░██─██░░██████████─██░░██████░░██───██░░██████████─██░░██████████─██░░████████░░██───██░░██──██░░██─\n\
─██░░██──────────██░░██─██░░██─────────██░░██──██░░██───██░░██─────────██░░██─────────██░░██────██░░██───██░░██──██░░██─\n\
─██░░██──██████──██░░██─██░░██████████─██░░██████░░████─██░░██████████─██░░██████████─██░░████████░░██───██░░██──██░░██─\n\
─██░░██──██░░██──██░░██─██░░░░░░░░░░██─██░░░░░░░░░░░░██─██░░░░░░░░░░██─██░░░░░░░░░░██─██░░░░░░░░░░░░██───██░░██──██░░██─\n\
─██░░██──██░░██──██░░██─██░░██████████─██░░████████░░██─██████████░░██─██░░██████████─██░░██████░░████───██░░██──██░░██─\n\
─██░░██████░░██████░░██─██░░██─────────██░░██────██░░██─────────██░░██─██░░██─────────██░░██──██░░██─────██░░░░██░░░░██─\n\
─██░░░░░░░░░░░░░░░░░░██─██░░██████████─██░░████████░░██─██████████░░██─██░░██████████─██░░██──██░░██████─████░░░░░░████─\n\
─██░░██████░░██████░░██─██░░░░░░░░░░██─██░░░░░░░░░░░░██─██░░░░░░░░░░██─██░░░░░░░░░░██─██░░██──██░░░░░░██───████░░████───\n\
─██████──██████──██████─██████████████─████████████████─██████████████─██████████████─██████──██████████─────██████─────\n\
────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────​​​​​\n\n\033[0m";
}
