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
	int									errorCode;
	bool								isRequestLineParsed;
	bool								isHeadersParsed;
	bool								isBodyParsed;

	bool	validateMethod(std::string method);

	public:
		Request();
		~Request();
		void	parseRequestLine(std::string requestLine);
		void	parseHeaders(std::string headersContent);
		void	parseBody();
		void	parse(std::string const requestData, Config *config);
		bool	isParsed();
		bool	isMultiPart();
		int 	getErrorCode();
		std::string							data;
		std::string							method;
		std::string							uri;
		std::string							httpVersion;
		std::map<std::string, std::string>	header;
		std::string							body;
		long double							contentLength;
		void	setErrorCode(int code);
};

#endif
