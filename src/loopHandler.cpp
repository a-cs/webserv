#include "../include/server.hpp"
#include "../include/epollHandler.hpp"

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
				std::cout << "Handle data from clients\n";
				char buffer[1024];
				bzero(buffer, sizeof(char)*1024);
				ssize_t bytesRead = read(epollHandler->events[i].data.fd, buffer, sizeof(buffer));

				std::cout << "bytesRead = " << bytesRead << "\n";
				std::cout << "buffer = \n----\n" << buffer << "\n----\n\n";
				if (bytesRead <= 0) {
					close(epollHandler->events[i].data.fd);
				} else {
					char response[] = "HTTP/1.1 200\nContent-Type: text/html\n\nHello World";

					write(epollHandler->events[i].data.fd, &response, sizeof(response)-1);
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
