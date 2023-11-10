#include "../include/server.hpp"
#include "../include/epollHandler.hpp"
#include "../include/request.hpp"
#include <sstream>

bool isRunning(bool status) {
	static bool running = true;

	if (!status)
		running = false;
	return running;
}

void stop(int sig) {
	if (sig == SIGINT || sig == SIGQUIT){
		std::cerr << "parou\n";
		isRunning(false);
	}
}

int	start(Server server, EpollHandler *epollHandler) {
	std::signal(SIGINT, stop);
	std::signal(SIGQUIT, stop);

	while (isRunning(true)) {
		int numEvents = epoll_wait(epollHandler->epollFd, epollHandler->events, MAX_EVENTS, 0);

		if (numEvents == -1) {
			std::cerr << "epoll_wait";
			return EXIT_FAILURE;
		}

		for (int i = 0; i < numEvents; i++) {
			if (epollHandler->events[i].data.fd == server.getSock()) {
				struct sockaddr_in clientAddr;
				socklen_t clientAddrLen = sizeof(clientAddr);
				int clientSocket = accept(server.getSock(), (struct sockaddr *)&clientAddr, &clientAddrLen);

				std::cout << "Accept incoming connections ";

				if (clientSocket == -1) {
					std::cerr << "accept";
				} else {
					struct epoll_event event;
					std::cout << clientSocket << "\n";
					event.events = EPOLLIN | EPOLLOUT;
					event.data.fd = clientSocket;
					epoll_ctl(epollHandler->epollFd, EPOLL_CTL_ADD, clientSocket, &event);
				}
			} else {
				std::string requestData = server.getRequestData(epollHandler->events[i].data.fd);
				std::cout << "\nREQ:\n" << requestData << "|||\n";
				Request req;
				req.parse(requestData);

				if (requestData.size() != 0) {
					std::stringstream response;
					if(req.getErrorCode() != 0)
						response << "HTTP/1.1 505\nContent-Type: text/html\n\nError";
					else
						response << "HTTP/1.1 200\nContent-Type: text/html\n\nHello World";
					std::string responseMessage = response.str();
					write(epollHandler->events[i].data.fd, responseMessage.c_str(), responseMessage.size());
					close(epollHandler->events[i].data.fd);
				}
			}
		}
	}
	return EXIT_SUCCESS;
}

int	run(){
	Server server;
	EpollHandler epollHandler;

	if (epollHandler.epollFd == -1) {
		std::cerr << "Failed to create epoll";
		return EXIT_FAILURE;
	}

    if (server.create() != 0)
		return EXIT_FAILURE;

    struct epoll_event event;
    event.events = EPOLLIN | EPOLLOUT;
    event.data.fd = server.getSock();

    if (epoll_ctl(epollHandler.epollFd, EPOLL_CTL_ADD, server.getSock(), &event) == -1) {
        std::cerr << "Failed to add server socket to epoll";
        return EXIT_FAILURE;
    }

	int v = start(server, &epollHandler);
	if(v != 1){}

	return 0;
}
