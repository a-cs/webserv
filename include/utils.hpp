#ifndef UTILS_HPP
# define UTILS_HPP

#include <iostream>
#include <vector>
#include <sys/stat.h>
#include <fstream>
#include <sstream>


namespace utils {
	std::vector<std::string>	split(std::string s, std::string delimiter);
	std::string					trim(std::string s);
	bool						isNumber(const std::string &s);
	bool						endsWith(const std::string &s, const std::string &c);
	bool						startsWith(const std::string &s, const std::string &c);
	bool						isFile(const std::string &s);
	std::string			getFile(std::string path);
}

#endif