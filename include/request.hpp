#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <iostream>
#include <stdio.h>
#include "utils.hpp"

#define CRLF "\r\n"


class Request {
	private:
	std::string	method;
	std::string	uri;
	std::string	httpVersion;
	std::string	body;
	int			errorCode;

	public:
		Request();
		~Request();
		void	parse(std::string const requestData);
		int 	getErrorCode();

};

#endif
