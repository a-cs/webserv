#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>
#include <vector>

class Location {
	public:
		bool						isDirectoryEnable;
		std::string					cgi;
		std::string					path;
		std::string					cgiPass;
		std::string					redirect;
		std::string					uploadPath;
		std::vector<std::string>	indexList;
		std::vector<std::string>	allowedMethods;

		Location();
		~Location();
};

#endif
