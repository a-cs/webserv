#include "../include/server.hpp"

int	run(){
	Server server;
	
	if (server.create() != 0)
		return EXIT_FAILURE;
	return server.start();
}

int main(int argc, char** argv) {
	if (argc != 2 && argv[0] != NULL) {
		std::cout << "Error: invalid arguments, you must provide only a file name as an argument.\n";
		return 1;
	} else {
		return run();
	}
	
}
