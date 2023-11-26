#ifndef UTILS_HPP
# define UTILS_HPP

#include <iostream>
#include <vector>
#include <sys/stat.h>
#include <fstream>
#include <sstream>

namespace utils {
	bool						pathExists(std::string path);
	bool						isFile(const std::string &s);
	bool						isNumber(const std::string &s);
	bool						isDirectory(const std::string &path);
	bool						endsWith(const std::string &s, const std::string &c);
	bool						startsWith(const std::string &s, const std::string &c);
	std::string					trim(std::string s);
	std::string					formatSize(size_t size);
	std::string					getFile(std::string path);
	std::vector<std::string>	split(std::string s, std::string delimiter);
}

#endif
