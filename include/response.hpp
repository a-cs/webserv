#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <iostream>
#include <sstream>
#include "utils.hpp"

#define CRLF "\r\n"
#define SP " "


class Response {
	private:
	std::string	httpVersion;
	std::string	body;

	void	renderErrorPage();

	public:
		Response();
		~Response();
		int			statusCode;
		void		setStatusCode(int newStatusCode);
		int			getStatusCode();
		std::string	getReasonPhrase();
		std::string	getMessage();
		void		setBody(std::string content);
};

#endif
