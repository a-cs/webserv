#include "server.hpp"

Server::Server(){
	port = 8080;
}

Server::~Server(){
	close(sock);
}

int Server::getSock() {
	return sock;
}

int	Server::create() {
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

	return EXIT_SUCCESS;
}


std::string	Server::getRequestData(int fd) {
	std::cout << "Getting request data\n";
	char buffer[1024];
	bzero(buffer, sizeof(char)*1024);
	ssize_t bytesRead = read(fd, buffer, sizeof(buffer) - 1);
	if (bytesRead <= 0){
		close(fd);
		return "";
	}
	return std::string(buffer);
}