#ifndef RESPONSE_HPP
# define RESPONSE_HPP

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
	std::string	httpVersion;
	std::string	body;
	std::map<std::string, std::string>	header;

	void	renderErrorPage();

	public:
		Response();
		~Response();
		int			statusCode;
		void		setStatusCode(int newStatusCode, Config &config);
		int			getStatusCode();
		std::string	getReasonPhrase();
		std::string	getMessage();
		void		setBody(std::string content);
		void		setHeader(std::string key, std::string value);
		void		setContentType(const std::string &fileExtenstion);
		void		renderDirectory(std::string root, std::string path);
};

#endif
