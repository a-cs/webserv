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
	port = config.port;
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
	std::cout << "fd=" << fd << "\n";
	char buffer[2048];
	bzero(buffer, sizeof(char)*2048);
	ssize_t bytesRead = read(fd, buffer, sizeof(buffer) - 1);
	if (bytesRead <= 0){
		close(fd);
		return "";
	}
	buffer[bytesRead] = 0;
	return std::string(buffer, bytesRead);
}

void	Server::handleMultipart(Request *request){
	std::string	body = request->body;
	std::string	contentType = request->header.at("content-type");
	std::string	boundary = "--" + contentType.substr(contentType.find("boundary=") + 9);
	std::string	uploadPath = "";
	for (size_t i = 0; i < config.locationList.size(); i++){
		if(config.locationList[i].path == request->uri){
			uploadPath = config.root + config.locationList[i].uploadPath;
		}
	}

	if(!utils::pathExists(uploadPath))
		mkdir(uploadPath.c_str(), 0777);

	size_t	pos = 0;
	size_t	endPos;
	std::cout << "\n\nEntrando while boundary:\n";
	while ((pos = body.find(boundary, pos)) != std::string::npos)
	{
		std::cout << "\nboundarySize:" << boundary.size() << "\n";
		pos += boundary.size();
		if(body.substr(pos, 2) == "--")
			break;
		pos += 2;
		endPos = body.find(boundary, pos);
		if(endPos == std::string::npos)
			endPos = body.size();
		std::string	part = body.substr(pos, endPos - pos);
		std::cout << "\npos:" << pos << "\n";
		std::cout << "\nendPos:" << endPos << "\n";
		std::cout << "\nfind:" << part.find("filename\"") <<"|\n";
		std::cout << "\npart:|" << part << "|\n";
		if(part.find("filename=\"") != std::string::npos){
			size_t	fileNameStart = part.find("filename=\"") + 10;
			size_t	fileNameEnd = part.find("\"", fileNameStart);
			std::string	fileName = part.substr(fileNameStart, fileNameEnd - fileNameStart);
			size_t	fileContentStart = part.find("\r\n\r\n") + 4;
			size_t	fileContentEnd = part.rfind("\r\n");
			std::string	fileContent = part.substr(fileContentStart, fileContentEnd - fileContentStart);
			std::string	filePath = uploadPath + "/" + fileName;
			std::ofstream file(filePath.c_str());
			file << fileContent;
			file.close();
			pos = endPos;
			std::cout << "\n\nCriou file:" << fileName << "|\n\n";
			std::cout << "\nnovo pos:" << pos << "\n";
		}
		else {
			pos = endPos;
		}
	}
}

void	Server::handleRequest(Request *request, Response *response){
	std::cout << "req errorcode=" << request->getErrorCode() << "\n";

	std::cout << "REQ body=" << request->body << "|\n";
	if(request->getErrorCode() != 0){
		response->setStatusCode(request->getErrorCode());
		return;
	}
	std::cout << "if1|\n";

	if (config.isRedirection(request->uri)) {
		std::string redirect = config.getRedirection(request->uri);
		response->setStatusCode(303);
		response->setHeader("Location", redirect);
		return;
	}
	std::cout << "if2|\n";


	if(request->isMultiPart()){
		handleMultipart(request);
		response->setStatusCode(201);
		return;
	}
	std::cout << "if3|\n";


	if(!utils::pathExists(this->config.root + request->uri)){
		response->setStatusCode(404);
		return;
	}
	std::cout << "if4|\n";


	if (request->body.size() > config.bodySizeLimit){
		response->setStatusCode(413);
		return;
	}

	std::cout << "ifcgi=" << config.isValidCgiRequest(request->uri) << "|\n";

	// lidando com cgi
	if(config.isValidCgiRequest(request->uri)) {
		Cgi cgi((config.root + request->uri + "/" + config.getCgiFile(request->uri)), this->config.port, request);
		std::string result = cgi.exec();
		std::cout << "\n\nResultCGI=" << result <<"|\n";
		std::cout << "find=" << result.find("Error") <<"|\n\n";
		if (result.find("Error") != std::string::npos) {
			response->setStatusCode(500);
		}
		response->setContentType("json");
		response->setBody(result);
		return;
	}

	std::cout << "\n\nifDIR=" << utils::isDirectory(this->config.root + request->uri) << "\n\n";

	if(utils::isDirectory(this->config.root + request->uri)){
		bool	notFound = true;
		for(size_t i = 0; i < config.locationList.size(); i++) {
			if (notFound && config.locationList[i].path == request->uri) {
				for(size_t j = 0; j < config.locationList[i].indexList.size(); j++){
					std::string path = utils::endsWith(this->config.root + request->uri, "/") ?
											this->config.root + request->uri + config.locationList[i].indexList[j] :
											this->config.root + request->uri + "/" + config.locationList[i].indexList[j];
					if(utils::isFile(path)){
						response->setBody(utils::getFile(path));
						notFound = false;
						break;
					}
				}
			}
		}

		if(notFound && request->uri != "/"){
			for(size_t i = 0; i < config.locationList.size(); i++) {
				if (config.locationList[i].path == request->uri) {
					if(config.locationList[i].isDirectoryEnable){
						std::cout << "listar dir!\n\n";
						notFound = false;
					}
					break;
				}
			}
			if(notFound)
				response->setStatusCode(404);
		}
		return;
	}

	if(utils::isFile(this->config.root + request->uri)) {
		if (request->method == "DELETE") {
			if (remove((this->config.root + request->uri).c_str()) != 0) {
				request->setErrorCode(400);
			}
		}
		else {
			response->setBody(utils::getFile(this->config.root + request->uri));
			std::string extension = request->uri.substr(request->uri.rfind(".") + 1);
			response->setContentType(extension);
		}
		return;
	}
}
