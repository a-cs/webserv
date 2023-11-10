#include "request.hpp"

Request::Request(): errorCode(0){
}

Request::~Request(){
}

int	Request::getErrorCode(){
	return errorCode;
}

void	Request::parse(std::string const requestData){
	size_t pos = requestData.find(CRLF);
	if(pos == std::string::npos)
		return;
	std::string requestLine = requestData.substr(0, pos);
	std::vector<std::string> requestLineTokens = utils::split(requestLine, " "); 
	if(requestLineTokens.size() != 3){
		errorCode = 500;
		std::cerr << "invalid request line\n";
	}
	else {

		method = requestLineTokens[0];
		uri = requestLineTokens[1];
		httpVersion = requestLineTokens[2];
		std::cout << "\nmethod=" << method <<"\n";
		std::cout << "uri=" << uri <<"\n";
		std::cout << "httpVersion=" << httpVersion <<"\n";

		if(httpVersion.compare("HTTP/1.1") != 0){
			errorCode = 505;
			std::cerr << "invalid HTTP protocol version\n";
		}
	}
}