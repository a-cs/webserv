#ifndef UTILS_HPP
# define UTILS_HPP

#include <iostream>
#include <vector>


namespace utils {
	std::vector<std::string>	split(std::string s, std::string delimiter);
	std::string					trim(std::string s);
	bool						isNumber(const std::string &s);
}

#endif