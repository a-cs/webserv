#include "server.hpp"

Server::Server() {
	port = 8080;
}

Server::~Server() {
	close(sock);
}

int Server::getSock() {
	return sock;
}

int Server::create() {
	port = config.port;
	sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock < 0) {
		std::cerr << "Error: Creaing Socket\n" << std::flush;
		return EXIT_FAILURE;
	}

	int yes = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		std::cerr << "Error: Set Socket option failed\n" << std::flush;
		return -1;
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sock, (sockaddr *)&addr, sizeof(addr)) < 0) {
		std::cerr << "Error: Could not bind the socket to port " << port << "\n" << std::flush;
		return EXIT_FAILURE;
	}
	if (listen(sock, 5) < 0) {
		std::cerr << "Error: Listen failed\n" << std::flush;
	}

	std::cout << "\e\033[92mServer listen on port: \033[0m" << this->config.port << "\n\n";
	return EXIT_SUCCESS;
}

std::string Server::getRequestData(int fd) {
	char buffer[2048];
	bzero(buffer, sizeof(char) * 2048);
	ssize_t bytesRead = read(fd, buffer, sizeof(buffer) - 1);
	if (bytesRead <= 0) {
		close(fd);
		return "";
	}
	buffer[bytesRead] = 0;
	return std::string(buffer, bytesRead);
}

void Server::handleMultipart(Request *request) {
	std::string body = request->body;
	std::string contentType = request->header.at("content-type");
	std::string boundary = "--" + contentType.substr(contentType.find("boundary=") + 9);
	std::string uploadPath = "";
	for (size_t i = 0; i < config.locationList.size(); i++) {
		if (config.locationList[i].path == request->uri)
			uploadPath = config.root + config.locationList[i].uploadPath;
	}

	if (!utils::pathExists(uploadPath))
		mkdir(uploadPath.c_str(), 0777);

	size_t pos = 0;
	size_t endPos;
	while ((pos = body.find(boundary, pos)) != std::string::npos) {
		pos += boundary.size();
		if (body.substr(pos, 2) == "--")
			break;
		pos += 2;
		endPos = body.find(boundary, pos);
		if (endPos == std::string::npos)
			endPos = body.size();
		std::string part = body.substr(pos, endPos - pos);
		if (part.find("filename=\"") != std::string::npos) {
			size_t fileNameStart = part.find("filename=\"") + 10;
			size_t fileNameEnd = part.find("\"", fileNameStart);
			std::string fileName = part.substr(fileNameStart, fileNameEnd - fileNameStart);
			size_t fileContentStart = part.find("\r\n\r\n") + 4;
			size_t fileContentEnd = part.rfind("\r\n");
			std::string fileContent = part.substr(fileContentStart, fileContentEnd - fileContentStart);
			std::string filePath = uploadPath + "/" + fileName;
			std::ofstream file(filePath.c_str());
			file << fileContent;
			file.close();
			pos = endPos;
		} else
			pos = endPos;
	}
}

void Server::handleRequest(Request *request, Response *response) {
	bool foundHost = false;
	for (size_t i = 0; i < config.serverNamesList.size(); i++) {
		if (config.serverNamesList[i] == request->header["host"].substr(0, request->header["host"].find(":"))) {
			foundHost = true;
			break;
		}
	}
	if (!foundHost) {
		response->setStatusCode(403, config);
		return;
	}

	if (request->getErrorCode() != 0) {
		response->setStatusCode(request->getErrorCode(), config);
		return;
	}

	if (config.isRedirection(request->uri)) {
		std::string redirect = config.getRedirection(request->uri);
		response->setStatusCode(303, config);
		response->setHeader("Location", redirect);
		return;
	}

	if (request->isMultiPart()) {
		handleMultipart(request);
		response->setStatusCode(201, config);
		return;
	}

	if (!utils::pathExists(this->config.root + request->uri)) {
		response->setStatusCode(404, config);
		return;
	}

	if (request->body.size() > config.bodySizeLimit) {
		response->setStatusCode(413, config);
		return;
	}

	if (config.isValidCgiRequest(request->uri)) {
		Cgi cgi((config.root + request->uri + "/" + config.getCgiFile(request->uri)), this->config.port, request);
		std::string result = cgi.exec();
		if (result.find("Error") != std::string::npos)
			response->setStatusCode(500, config);
		if (result == ""){
			response->setStatusCode(508, config);
			result = "{\"Error\": \"Loop Detected\"}";
		}
		response->setContentType("json");
		response->setBody(result);
		return;
	}

	if (utils::isDirectory(this->config.root + request->uri)) {
		bool notFound = true;
		for (size_t i = 0; i < config.locationList.size(); i++) {
			if (notFound && config.locationList[i].path == request->uri) {
				for (size_t j = 0; j < config.locationList[i].indexList.size(); j++) {
					std::string path = utils::endsWith(this->config.root + request->uri, "/") ? this->config.root + request->uri + config.locationList[i].indexList[j] : this->config.root + request->uri + "/" + config.locationList[i].indexList[j];
					if (utils::isFile(path)) {
						response->setBody(utils::getFile(path));
						notFound = false;
						break;
					}
				}
			}
		}

		if (notFound && request->uri != "/") {
			for (size_t i = 0; i < config.locationList.size(); i++) {
				if (config.locationList[i].path == request->uri) {
					if (config.locationList[i].isDirectoryEnable) {
						response->renderDirectory(config.root, request->uri);
						notFound = false;
						break;
					}
				}
			}
			if (notFound)
				response->setStatusCode(404, config);
		}
		return;
	}

	if (utils::isFile(this->config.root + request->uri)) {
		if (request->method == "DELETE") {
			if (remove((this->config.root + request->uri).c_str()) != 0)
				request->setErrorCode(400);
		} else {
			response->setBody(utils::getFile(this->config.root + request->uri));
			std::string extension = request->uri.substr(request->uri.rfind(".") + 1);
			response->setContentType(extension);
		}
		return;
	}
}
