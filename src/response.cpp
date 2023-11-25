#include "response.hpp"

Response::Response(): httpVersion("HTTP/1.1"), statusCode(200){
}

Response::~Response(){
}

void Response::setBody(std::string content){
	body = content;
}

void	Response::setHeader(std::string key, std::string value){
	header[key] = value;
}

void	Response::renderErrorPage(){
	std::stringstream	ss;

	ss << "<html>";
	ss << "<head>";
	ss << "<title>Document</title>";
	ss << "</head>";
	ss << "<style>";
	ss << "html{font-size: 16px;}";
	ss << "body{line-height: 1;height: 100vh;display: flex;flex-direction: column;justify-content: center;align-items: center;color:#E71D36;background-color: #0d1821;}";
	ss << ".box{width: 600px;height: 600px;display: flex;flex-direction: column;justify-content: center;align-items: center;background-color: #fdfffc;border-radius: 16px;}";
	ss << "h1{margin: 0;margin-bottom: 2rem;font-size: 7rem;}";
	ss << "h2{margin: 0;font-size: 3.5rem;text-decoration: underline;}";
	ss << "</style>";
	ss << "<body>";
	ss << "<div class='box'>";
	ss << "<h1>"<< statusCode <<"</h1>";
	ss << "<h2>" << getReasonPhrase() << "</h2>";
	ss << "</div>";
	ss << "</body>";
	ss << "</html>";

	setBody(ss.str());
}

void	Response::setStatusCode(int newStatusCode, Config &config){
	statusCode = newStatusCode;

	if(statusCode >= 400){
		std::map<int, std::string>:: iterator it;
		for(it = config.errorPageList.begin(); it != config.errorPageList.end(); ++it) {
			if (it->first == newStatusCode)
				setBody(utils::getFile(config.root + "/" + it->second));
			else
				renderErrorPage();
		}
	}
}

int	Response::getStatusCode(){
	return statusCode;
}

std::string	Response::getReasonPhrase(){
	switch (statusCode)
	{
		case 100:
			return "Continue";
		case 101:
			return "Switching Protocols";
		case 102:
			return "Processing";
		case 200:
			return "OK";
		case 201:
			return "Created";
		case 202:
			return "Accepted";
		case 203:
			return "Non-Authoritative Information";
		case 204:
			return "No Content";
		case 205:
			return "Reset Content";
		case 206:
			return "Partial Content";
		case 207:
			return "Multi-Status";
		case 208:
			return "Already Reported";
		case 226:
			return "IM Used";
		case 300:
			return "Multiple Choices";
		case 301:
			return "Moved Permanently";
		case 302:
			return "Found";
		case 303:
			return "See Other";
		case 304:
			return "Not Modified";
		case 305:
			return "Use Proxy";
		case 307:
			return "Temporary Redirect";
		case 308:
			return "Permanent Redirect";
		case 400:
			return "Bad Request";
		case 401:
			return "Unauthorized";
		case 402:
			return "Payment Required";
		case 403:
			return "Forbidden";
		case 404:
			return "Not Found";
		case 405:
			return "Method Not Allowed";
		case 406:
			return "Not Acceptable";
		case 407:
			return "Proxy Authentication Required";
		case 408:
			return "Request Timeout";
		case 409:
			return "Conflict";
		case 410:
			return "Gone";
		case 411:
			return "Length Required";
		case 412:
			return "Precondition Failed";
		case 413:
			return "Payload Too Large";
		case 414:
			return "URI Too Long";
		case 415:
			return "Unsupported Media Type";
		case 416:
			return "Range Not Satisfiable";
		case 417:
			return "Expectation Failed";
		case 421:
			return "Misdirected Request";
		case 422:
			return "Unprocessable Entity";
		case 423:
			return "Locked";
		case 424:
			return "Failed Dependency";
		case 426:
			return "Upgrade Required";
		case 428:
			return "Precondition Required";
		case 429:
			return "Too Many Requests";
		case 431:
			return "Request Header Fields Too Large";
		case 451:
			return "Unavailable For Legal Reasons";
		case 500:
			return "Internal Server Error";
		case 501:
			return "Not Implemented";
		case 502:
			return "Bad Gateway";
		case 503:
			return "Service Unavailable";
		case 504:
			return "Gateway Timeout";
		case 505:
			return "HTTP Version Not Supported";
		case 506:
			return "Variant Also Negotiates";
		case 507:
			return "Insufficient Storage";
		case 508:
			return "Loop Detected";
		case 510:
			return "Not Extended";
		case 511:
			return "Network Authentication Required";
		default:
			return "Error";
	}
}

void Response::setContentType(const std::string &fileExtenstion)
{
	if (fileExtenstion == "html")
		setHeader("Content-Type", "text/html; charset=UTF-8");
	else if (fileExtenstion == "css")
		setHeader("Content-Type", "text/css; charset=UTF-8");
	else if (fileExtenstion == "js")
		setHeader("Content-Type", "text/javascript; charset=UTF-8");
	else if (fileExtenstion == "jpg" || fileExtenstion == "jpeg")
		setHeader("Content-Type", "image/jpeg");
	else if (fileExtenstion == "png")
		setHeader("Content-Type", "image/png");
	else if (fileExtenstion == "ico")
		setHeader("Content-Type", "image/x-icon");
	else if (fileExtenstion == "txt")
		setHeader("Content-Type", "text/plain");
	else if (fileExtenstion == "json")
		setHeader("Content-Type", "application/json");
	else
		setHeader("Content-Type", "application/octet-stream");
}

void	Response::renderDirectory(std::string root, std::string path){
	DIR	*dir;

	struct dirent	*ent;
	struct stat		fileStat;
	std::string		fullPath, modifiedTime;
	std::stringstream	ss;
	if(path[path.size() - 1] != '/')
		path += '/';
	ss << "<html>"
		<< "<head>"
		<< "<title>Index of " << path << "</title>"
		<< "</head>"
		<< "<body class=\"container\">"
		<< "<h1>Index of " << path << "</h1>"
		<< "<table>"
		<< "<tr><th>Name</th><th>Size</th><th>Date Modified</th></tr>";

	std::string dirPath = root + path;
	dir = opendir(dirPath.c_str());
	while((ent = readdir(dir)) != NULL){
		if(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
			continue;
		fullPath = dirPath + "/" + ent->d_name;
		if(stat(fullPath.c_str(), &fileStat) == -1)
			continue;
		modifiedTime = ctime(&fileStat.st_mtime);
		ss << "<tr>";
		if(utils::isDirectory(fullPath)){
			ss << "<td><a href=\"" << path << ent->d_name << "\">" << ent->d_name
				<< "/</a></td>";
			ss << "<td>-</td>";
		}
		else {
			ss << "<td><a href=\"" << path << ent->d_name << "\">" << ent->d_name
			   << "</a></td>";
			ss << "<td>" << utils::formatSize(fileStat.st_size) << "</td>";
		}
		ss << "<td>" << modifiedTime;
		ss << "</td></tr>";
	}
	closedir(dir);
	ss << "</table>"
	   << "</body>"
	   << "</html>";

	setBody(ss.str());
}

std::string	Response::getMessage(){
	std::stringstream	message;

	message << httpVersion << SP << statusCode << SP << getReasonPhrase() << CRLF;
	
	std::map<std::string, std::string>:: iterator it;

	for(it = header.begin(); it != header.end(); ++it){
		
		message << it->first << ": " << it->second << CRLF;
	}

	message << CRLF;

	if (this->body.size() > 0)
		message << this->body;

	std::cout << "MESSAGE:\n" << message.str() << "|||\n";

	return message.str();
}
