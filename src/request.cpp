#include "request.hpp"

Request::Request(): errorCode(0), isRequestLineParsed(false), isHeadersParsed(false){
}

Request::~Request(){
}

int	Request::getErrorCode(){
	return errorCode;
}

void	Request::parseRequestLine(std::string requestLine){
	std::vector<std::string> requestLineTokens = utils::split(requestLine, " "); 
	if(requestLineTokens.size() != 3){
		errorCode = 500;
		std::cerr << "invalid request line\n";
		return;
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
			return;
		}
	}
	isRequestLineParsed = true;
}

void	Request::parseHeaders(std::string headersContent){
	std::cout << "\n\nHeader|" << headersContent << "|\n\n";
	std::istringstream	iss(headersContent);
	std::string			line;
	size_t				pos;

	while(std::getline(iss, line)){
		line = utils::trim(line);
		if(line.empty()){
			isHeadersParsed = true;
			std::cout << "\nBREAK";
			break;
		}
		pos = line.find(":");
		if(pos == std::string::npos){
			errorCode = 400;
			std::cerr << "bad request, invalid Header on line=\n" << line << "\n";
			break;
		}
		std::string	key = line.substr(0, pos);
		std::transform(key.begin(), key.end(), key.begin(), tolower);
		std::string	value = line.substr(pos + 1);
		value = utils::trim(value);
		if(value.empty()){
			errorCode = 400;
			std::cerr << "bad request, invalid Header value on line=\n" << line << "\n";
			break;
		}
		if(key == "content-length" && !utils::isNumber(value)){
			errorCode = 400;
			std::cerr << "bad request, invalid Content-Length value on line=\n" << line << "\n";
			break;
		}
		header[key] = value;

		if(errorCode == 0){
			isHeadersParsed = true;
		}
	}

}

void	Request::parse(std::string const requestData){
	std::string	buffer;

	size_t pos = requestData.find(CRLF);
	if(pos == std::string::npos)
		return;
	std::string requestLine = requestData.substr(0, pos);
	parseRequestLine(requestLine);
	if(!isRequestLineParsed)
		return;
	buffer = requestData.substr(pos + 2);
	std::cout << "\n\nBUFFER|" << buffer << "|\n\n";
	pos = buffer.find(CRLF CRLF);
	if(pos == std::string::npos){
		errorCode = 400;
		std::cout << "Header Missing\n";
		return;
	}
	std::string	headersContent = buffer.substr(0, pos);
	parseHeaders(headersContent);
	if(!isHeadersParsed)
		return;

	std::string	bodyContent = buffer.substr(pos + 4);
	std::cout << "\n\nBody|" << bodyContent << "|\n\n";
}