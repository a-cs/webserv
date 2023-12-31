#include "request.hpp"

Request::Request() {
	errorCode = 0;
	isRequestLineParsed = false;
	isHeadersParsed = false;
	isBodyParsed = false;
	contentLength = 0;
}

Request::~Request() {}

int Request::getErrorCode() {
	return errorCode;
}

void Request::setErrorCode(int code) {
	this->errorCode = code;
}

bool Request::validateMethod(std::string method) {
	for (size_t i = 0; i < config.locationList.size(); i++) {
		if (config.locationList[i].path == uri) {
			if (std::find(config.locationList[i].allowedMethods.begin(), config.locationList[i].allowedMethods.end(), method) == config.locationList[i].allowedMethods.end()) {
				errorCode = 405;
				return false;
			}
			break;
		}
	}
	return true;
}

void Request::parseRequestLine(std::string requestLine) {
	std::vector<std::string> requestLineTokens = utils::split(requestLine, " ");
	if (requestLineTokens.size() != 3) {
		errorCode = 500;
		return;
	} else {
		method = requestLineTokens[0];
		uri = requestLineTokens[1];
		httpVersion = requestLineTokens[2];

		if (!validateMethod(method))
			return;

		if (httpVersion.compare("HTTP/1.1") != 0) {
			errorCode = 505;
			return;
		}
	}
	isRequestLineParsed = true;
}

void Request::parseHeaders(std::string headersContent)
{
	std::istringstream iss(headersContent);
	std::string line;
	size_t pos;

	while (std::getline(iss, line)) {
		line = utils::trim(line);
		if (line.empty()) {
			isHeadersParsed = true;
			break;
		}
		pos = line.find(":");
		if (pos == std::string::npos) {
			errorCode = 400;
			break;
		}
		std::string key = line.substr(0, pos);
		std::transform(key.begin(), key.end(), key.begin(), tolower);
		std::string value = line.substr(pos + 1);
		value = utils::trim(value);
		if (value.empty()) {
			errorCode = 400;
			break;
		}
		if (key == "content-length" && !utils::isNumber(value)) {
			errorCode = 400;
			break;
		}
		if (key == "content-length") {
			std::stringstream ss(value);
			ss >> contentLength;
		}
		header[key] = value;

		if (errorCode == 0)
			isHeadersParsed = true;
	}
}

void Request::parseBody() {
	if (header.find("transfer-encoding") != header.end()) {
		std::string transferEncoding = header.at("transfer-encoding");
		if (transferEncoding == "chunked") {
			size_t chuckSizeStrPos = data.find(CRLF);
			std::string chuckSizeStr = data.substr(0, chuckSizeStrPos);
			std::stringstream ss(chuckSizeStr);
			std::size_t chuckSize = 0;
			ss >> std::hex >> chuckSize;
			if (data.find("\r\n0\r\n") == 0 || data.find("0\r\n") == 0) {
				isBodyParsed = true;
				return;
			}
			if (data.size() < chuckSize)
				return;
			data.erase(0, chuckSizeStr.size() + 2);
			body.append(data.substr(0, chuckSize));
			data.erase(0, chuckSize);
			if (data.find("\r\n0\r\n") == 0 || data.find("0\r\n") == 0) {
				isBodyParsed = true;
				return;
			}
			if (data.find(CRLF) == 0)
				data.erase(0, 2);
			if (data.size() > 0)
				parseBody();
			return;
		}
	}
	if (contentLength > 0) {
		if (data.size() > config.bodySizeLimit) {
			errorCode = 413;
			return;
		}
		if (data.size() < contentLength)
			return;
		body = data.substr(0, contentLength);
		data.erase(0, contentLength);
		isBodyParsed = true;
		return;
	}
	isBodyParsed = true;
}

bool Request::isParsed() {
	if (errorCode != 0)
		return true;
	return isRequestLineParsed && isHeadersParsed && isBodyParsed;
}

void Request::parse(std::string const requestData, Config *config) {
	data += requestData;

	size_t pos = requestData.find(CRLF);
	this->config = *config;
	if (!isRequestLineParsed) {
		if (pos == std::string::npos)
			return;
		std::string requestLine = requestData.substr(0, pos);
		parseRequestLine(requestLine);
		data = requestData.substr(pos + 2);
		if (!isRequestLineParsed)
			return;
	}
	if (!isHeadersParsed) {
		pos = data.find(CRLF CRLF);
		if (pos == std::string::npos) {
			errorCode = 400;
			return;
		}
		std::string headersContent = data.substr(0, pos);
		parseHeaders(headersContent);
		data = data.substr(pos + 4);
		if (!isHeadersParsed)
			return;
	}

	if (!isBodyParsed) {
		std::string bodyContent = data.substr(0, pos);
		parseBody();
		if (!isBodyParsed)
			return;
	}
}

bool Request::isMultiPart()
{
	if (header.find("content-type") != header.end()) {
		std::string contentType = header.at("content-type");
		if (contentType.find("multipart/form-data") != std::string::npos)
			return true;
	}
	return false;
}
