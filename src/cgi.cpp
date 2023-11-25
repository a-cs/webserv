#include "../include/cgi.hpp"

Cgi::Cgi(std::string const &fullPath, Server &server)
    : fullPath(fullPath), server(server) {}

Cgi::~Cgi() {}

int Cgi::exec() {
    if (access(this->fullPath.c_str(), R_OK) != 0)
		return 500;
	
	std::signal(SIGCHLD, SIG_IGN);
	int pid = fork();
	if (pid == 0) {
		std::stringstream ss;
		ss << server.config.port;
		this->port = ss.str();

		// prepareCGIRequest();
		// executeCGIScript();

		return 0;
	}
    return 0;
}
