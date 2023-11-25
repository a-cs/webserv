#ifndef CGI_HPP
#define CGI_HPP

#include <string>
#include "request.hpp"

class Cgi
{
	private:
        std::string _fullPath;
        Request &_request;
	public:
		Cgi(std::string const &fullPath, Request &request);
		~Cgi();
        
        int exec();
};

#endif
