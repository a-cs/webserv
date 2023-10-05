#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

int main(void) {
	int sock, client;
	sock= socket(AF_INET, SOCK_STREAM, 0);

	if(sock < 0) {
		std::cerr << "Socket nao pode ser criado\n" << std::flush;
		return EXIT_FAILURE;
	}
	struct sockaddr_in addr, client_addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi("8080"));
	addr.sin_addr.s_addr = INADDR_ANY;
	bind(sock, (sockaddr *) &addr, sizeof(addr));
	listen(sock, 1000);
	std::cout << "Escutando na porta 8080\n" << std::flush;
	char response[] = "HTTP/1.1 200\nContent-Type: text/html\n\nHello World";
	while(true) {
		socklen_t clilen = sizeof(client_addr);
		client = accept(sock, (struct sockaddr *) &client_addr, &clilen);
		std::cout << "Recebida conexao\n" << std::flush;
		if(client < 0) {
			std::cerr << "Socket nao pode ser aceito\n" << std::flush;
		}
		write(client, &response, sizeof(response)-1);
		close(client);
	}
	close(sock);

	return EXIT_SUCCESS;
}
