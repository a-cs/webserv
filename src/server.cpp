#include "server.hpp"

Server::Server(){}

Server::~Server(){
	close(sock);
}


bool isRunning(bool status) {
	static bool running = true;

	if (!status)
		running = false;
	return running;
}


void stop(int sig) {
	if (sig == SIGINT || sig == SIGQUIT)
		isRunning(false);
}



int	Server::create() {
	port = 8080;
	sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock < 0) {
		std::cerr << "Error: Creaing Socket\n" << std::flush;
		return EXIT_FAILURE;
	}

	int yes = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		std::cerr << "Error: Set Socket option failed\n" << std::flush;
		return -1;
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sock, (sockaddr *) &addr, sizeof(addr)) <0) {
		std::cerr << "Error: Could not bind the socket to port "<< port <<"\n" << std::flush;
		return EXIT_FAILURE;
	}
	if (listen(sock, 5) < 0) {
		std::cerr << "Error: Listen failed\n" << std::flush;
	}

	std::cout << "Server is listening on port "<< port <<"\n" << std::flush;

// Create an epoll instance
	epollFd = epoll_create(1);

	if (epollFd == -1) {
		std::cerr << "Failed to create epoll";
		return EXIT_FAILURE;
	}

	struct epoll_event event;
	event.data.fd = sock;
	event.events = EPOLLIN;

	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, sock, &event) == -1) {
		std::cerr << "Failed to add server socket to epoll";
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int	Server::start() {
	std::signal(SIGINT, stop);
	std::signal(SIGQUIT, stop);

	while (isRunning(true)) {
		int numEvents = epoll_wait(epollFd, events, MAX_EVENTS, 0);

		if (numEvents == -1) {
			std::cerr << "epoll_wait";
			return EXIT_FAILURE;
		}

		for (int i = 0; i < numEvents; i++) {
			if (events[i].data.fd == sock) {
				struct sockaddr_in clientAddr;
				socklen_t clientAddrLen = sizeof(clientAddr);
				int clientSocket = accept(sock, (struct sockaddr *)&clientAddr, &clientAddrLen);

				std::cout << "Accept incoming connections ";

				if (clientSocket == -1) {
					std::cerr << "accept";
				} else {
					std::cout << clientSocket << "\n";
					event.data.fd = clientSocket;
					event.events = EPOLLIN;
					epoll_ctl(epollFd, EPOLL_CTL_ADD, clientSocket, &event);
				}
			} else {
				std::cout << "Handle data from clients\n";
				char buffer[1024];
				bzero(buffer, sizeof(char)*1024);
				ssize_t bytesRead = read(events[i].data.fd, buffer, sizeof(buffer));

				std::cout << "bytesRead = " << bytesRead << "\n";
				std::cout << "buffer = \n----\n" << buffer << "\n----\n\n";
				if (bytesRead <= 0) {
					close(events[i].data.fd);
				} else {
					char response[] = "HTTP/1.1 200\nContent-Type: text/html\n\nHello World";

					write(events[i].data.fd, &response, sizeof(response)-1);
					close(events[i].data.fd);
				}
			}
		}
	}
	return EXIT_SUCCESS;
}
