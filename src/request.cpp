#include "request.hpp"

Request::Request(){
}

Request::~Request(){
}

void	Request::parse(std::string const requestData){
	size_t pos = requestData.find(CRLF);
	if(pos == std::string::npos)
		return;
	std::string requestLine = requestData.substr(0, pos);
	std::cout << "REQLINE =" << requestLine <<"|||\n";
	std::vector<std::string> requestLineTokens = utils::split(requestLine, " "); 
	if(requestLineTokens.size() != 3){
		std::cerr << "invalide request line\n";
	}
	method = requestLineTokens[0];
	uri = requestLineTokens[1];
	httpVersion = requestLineTokens[2];
	std::cout << "\nmethod=" << method <<"\n";
	std::cout << "uri=" << uri <<"\n";
	std::cout << "httpVersion=" << httpVersion <<"\n";
}