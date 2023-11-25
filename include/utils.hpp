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
	bool						isDirectory(const std::string &path);
	bool						isFile(const std::string &s);
	bool						pathExists(std::string path);
	std::string					getFile(std::string path);
	std::string					formatSize(size_t size);
}

#endif