#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <sstream>
#include <map>
#include <sys/types.h>
#include <dirent.h>
#include <cstring>
#include "utils.hpp"
#include "config.hpp"

#define CRLF "\r\n"
#define SP " "

class Response {
	private:
		std::string							body;
		std::string							httpVersion;
		std::map<std::string, std::string>	header;

		void renderErrorPage();

	public:
		int	statusCode;

		Response();
		~Response();

		int			getStatusCode();
		void		setBody(std::string content);
		void		setHeader(std::string key, std::string value);
		void		setStatusCode(int newStatusCode, Config &config);
		void		setContentType(const std::string &fileExtenstion);
		void		renderDirectory(std::string root, std::string path);
		std::string	getMessage();
		std::string	getReasonPhrase();
};

#endif
