#include "request.hpp"

Request::Request(): errorCode(0), isRequestLineParsed(false), isHeadersParsed(false), isBodyParsed(false){
	contentLength = 0;
}

Request::~Request(){
}

int	Request::getErrorCode(){
	return errorCode;
}

void	Request::setErrorCode(int code) {
	this->errorCode = code;
}

bool	Request::validateMethod(std::string method){
	for (size_t i = 0; i < config.locationList.size(); i++){
		if(config.locationList[i].path == uri){
			if(std::find(config.locationList[i].allowedMethods.begin(), config.locationList[i].allowedMethods.end(), method) == config.locationList[i].allowedMethods.end()){
				errorCode = 405;
				std::cerr << "Method Not Allowed\n";
				return false;
			}
			break;
		}
	}
	return true;
}

bool	Request::validateUri(std::string uri){
	for (size_t i = 0; i < config.locationList.size(); i++){
		if(config.locationList[i].path == uri){
			return true;
		}
	}
	errorCode = 404;
	std::cerr << "Not Found\n";
	return false;
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

		if(!validateUri(uri)){
			return;
		}

		if(!validateMethod(method)){
			return;
		}

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
		if(key == "content-length"){
			std::stringstream	ss(value);
			ss >> contentLength;
		}
		header[key] = value;

		if(errorCode == 0){
			isHeadersParsed = true;
		}
	}

}

void	Request::parseBody(){
	if(contentLength > 0){
		if(data.size() > config.bodySizeLimit)
		{
			errorCode = 413;
			std::cerr << "Payload too large\n";
			return;
		}
		if(data.size() < contentLength)
			return;
		body = data.substr(0, contentLength);
		data.erase(0, contentLength);
		isBodyParsed = true;
		return;
	}
	isBodyParsed = true;
}

bool	Request::isParsed(){
	if(errorCode != 0){
		return true;
	}
	return isRequestLineParsed && isHeadersParsed && isBodyParsed;
}

void	Request::parse(std::string const requestData, Config *config){
	data += requestData;

	size_t pos = requestData.find(CRLF);
	this->config = *config;
	if(!isRequestLineParsed){
		if(pos == std::string::npos)
			return;
		std::string requestLine = requestData.substr(0, pos);
		parseRequestLine(requestLine);
		data = requestData.substr(pos + 2);
		if(!isRequestLineParsed)
			return;
	}
	if(!isHeadersParsed){
		pos = data.find(CRLF CRLF);
		if(pos == std::string::npos){
			errorCode = 400;
			std::cout << "Header Missing\n";
			return;
		}
		std::string	headersContent = data.substr(0, pos);
		parseHeaders(headersContent);
		data = data.substr(pos + 4);
		if(!isHeadersParsed)
			return;
	}
	// std::cout << "\n\nBUFFER|" << buffer << "||\n\n";


	std::cout << "\n\nBodySize|" << data.size() << "|\n";
	if(!isBodyParsed){
		std::string	bodyContent = data.substr(0, pos);
		parseBody();
		if(!isBodyParsed){
			return;
		}
	}
}

bool Request::isMultiPart(){
	if(header.find("content-type") != header.end()){
		std::string contentType = header.at("content-type");
		if(contentType.find("multipart/form-data") != std::string::npos)
			return true;
	}
	return false;
}