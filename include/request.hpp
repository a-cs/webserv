#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <iostream>
#include <stdio.h>
#include <sstream>
#include <map>
#include <algorithm>
#include "utils.hpp"
#include "config.hpp"

#define CRLF "\r\n"


class Request {
	private:
	Config								config;
	std::string							method;
	std::string							uri;
	std::string							httpVersion;
	std::map<std::string, std::string>	header;
	std::string							body;
	int									errorCode;
	bool								isRequestLineParsed;
	bool								isHeadersParsed;

	bool	validateUri(std::string uri);
	bool	validateMethod(std::string method);

	public:
		Request();
		~Request();
		void	parseRequestLine(std::string requestLine);
		void	parseHeaders(std::string headersContent);
		void	parse(std::string const requestData, Config *config);
		int 	getErrorCode();

};

#endif
