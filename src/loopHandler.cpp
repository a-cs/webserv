#include "../include/server.hpp"
#include "../include/epollHandler.hpp"
#include "../include/request.hpp"
#include "../include/config.hpp"
#include <sstream>
#include <fcntl.h>

typedef struct
{
	std::string	type;
	int			fd;
	void		*ptr;
	Request		req;
} connection_t;

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

bool setNonBlocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		return false;
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		std::cerr << "nonBlocking";
		return false;
	}
	return true;
}

int	start(EpollHandler *epollHandler) {
	std::signal(SIGINT, stop);
	std::signal(SIGQUIT, stop);

	while (isRunning(true)) {
		int numEvents = epoll_wait(epollHandler->epollFd, epollHandler->events, MAX_EVENTS, 0);

		if (numEvents == -1) {
			std::cerr << "epoll_wait";
			return EXIT_FAILURE;
		}

		// std::cout << "numEvents" << numEvents << "\n";


		for (int i = 0; i < numEvents; i++) {
			std::string requestData;
			connection_t *connection = (connection_t *)(epollHandler->events[i].data.ptr);
			// std::cout << "connection fd " << connection->fd << "\n";
			// std::cout << "connection->type: " << connection->type<< "\n";
			Server *server = (Server *)(connection->ptr);
			// std::cout << "server :" << server->getSock() << "\n";
			// std::cout << "epollevent " << epollHandler->events[i].events << "\n";
			if (connection->type == "new connection") {
				std::cout << "if new\n";
				struct sockaddr_in clientAddr;
				socklen_t clientAddrLen = sizeof(clientAddr);
				int clientSocket = accept(server->getSock(), (struct sockaddr *)&clientAddr, &clientAddrLen);

				std::cout << "Accept incoming connections ";

				if (clientSocket == -1) {
					std::cerr << "\n Error accept";
				} else {
					setNonBlocking(clientSocket);
					connection_t *newConnection = new connection_t;
					Request req;
					newConnection->type ="connected";
					newConnection->ptr = server;
					newConnection->fd = clientSocket;
					newConnection->req = req;
					std::cout << "newConnection fd " << newConnection->fd << "\n";
					struct epoll_event event;
					std::cout << clientSocket << "\n";
					event.events = EPOLLIN;
					event.data.ptr = newConnection;
					epoll_ctl(epollHandler->epollFd, EPOLL_CTL_ADD, clientSocket, &event);
				}
				std::cout << "end if new\n";
				continue;
			}
			if(epollHandler->events[i].events & EPOLLIN){
				std::cout << "if req\n";
				requestData = server->getRequestData(connection->fd);
				std::cout << "\nREQ:\n" << requestData << "|||\n";
				
				connection->req.parse(requestData, &server->config);

				if(connection->req.isParsed()){
					connection->type ="response";
					// connection->req = req;

					struct epoll_event event;
					event.events = EPOLLOUT;
					// event.data.fd = connection->fd;
					event.data.ptr = connection;
					std::cout << "connection fd " << connection->fd << "\n";
					std::cout << "server :" << server->getSock() << "\n";
					std::cout << "epollevent " << epollHandler->events[i].events << "\n";
					epoll_ctl(epollHandler->epollFd, EPOLL_CTL_MOD, connection->fd, &event);
				}

				// connection_t *newConnection = new connection_t;
				// newConnection->type ="response";
				// newConnection->ptr = &server;
				// newConnection->fd = connection->fd;
				// newConnection->ptr = server;
				// newConnection->fd = connection->fd;

				std::cout << "end if req\n";
				continue;
			}
			if(epollHandler->events[i].events & EPOLLOUT){
				// flag= true;
				std::cout << "if res\n";
				Response res;

				// std::cout << "\n\nFINAL REQ BODY=" << connection->req.body <<"|\n\n";
	
				server->handleRequest(&connection->req, &res);
				std::string message = res.getMessage();
				std::cout << "connection fd " << connection->fd << "\n";
				std::cout << "server :" << server->getSock() << "\n";
				std::cout << "epollevent " << epollHandler->events[i].events << "\n";


				// std::stringstream response;
				// if(req.getErrorCode() != 0)
				// 	response << "HTTP/1.1 505\nContent-Type: text/html\n\nError";
				// else
				// 	response << "HTTP/1.1 200\nContent-Type: text/html\n\nHello World";
				// std::cout << "res connection fd " << connection->fd << "\n";
				// std::string responseMessage = response.str();
				write(connection->fd, message.c_str(), message.size());
				epoll_ctl(epollHandler->epollFd, EPOLL_CTL_DEL, connection->fd, 0);
				close(connection->fd);
				delete connection;

				std::cout << "end if res\n";
				continue;
			}
		}
	}
	return EXIT_SUCCESS;
}

int	run(std::vector<Config> configList){
	Server			servers[configList.size()];
	connection_t	connections[configList.size()];
	EpollHandler	epollHandler;

	if (epollHandler.epollFd == -1) {
		std::cerr << "Failed to create epoll";
		return EXIT_FAILURE;
	}

	for (size_t i = 0; i < configList.size(); i++) {
		servers[i].config = configList[i];
		if (servers[i].create() != 0)
			return EXIT_FAILURE;
		std::cout << "sock:" <<servers[i].getSock() << "\n";
		connection_t *connection = &connections[i];
		connection->type ="new connection";
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





	int v = start(&epollHandler);
	if(v != 1){}

	return 0;
}
