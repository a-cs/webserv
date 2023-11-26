#ifndef REQUEST_HPP
#define REQUEST_HPP

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
		int		errorCode;
		bool	isBodyParsed;
		bool	isHeadersParsed;
		bool	isRequestLineParsed;
		Config	config;

		bool	validateMethod(std::string method);

	public:
		std::string							uri;
		std::string							body;
		std::string							data;
		std::string							method;
		std::string							httpVersion;
		long double							contentLength;
		std::map<std::string, std::string>	header;

		Request();
		~Request();

		int		getErrorCode();
		bool	isParsed();
		void	parseBody();
		bool	isMultiPart();
		void	setErrorCode(int code);
		void	parseHeaders(std::string headersContent);
		void	parseRequestLine(std::string requestLine);
		void	parse(std::string const requestData, Config *config);
};

#endif
