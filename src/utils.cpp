#include "utils.hpp"

std::vector<std::string>	utils::split(std::string s, std::string delimiter){
    size_t start = 0, end, delimLen = delimiter.length();
    std::string token;
    std::vector<std::string> tokenList;

    while ((end = s.find(delimiter, start)) != std::string::npos) {
        token = s.substr (start, end - start);
        start = end + delimLen;
        tokenList.push_back (token);
    }

    tokenList.push_back (s.substr (start));
    return tokenList;
}

static std::string &rtrim(std::string &s, const char *ws)
{
	s.erase(s.find_last_not_of(ws) + 1);
	return s;
}

static std::string &ltrim(std::string &s, const char *ws)
{
	s.erase(0, s.find_first_not_of(ws));
	return s;
}

std::string	utils::trim(std::string s){
	const char	*ws = " \t\n\r\f\v";
	return ltrim(rtrim(s, ws), ws);
}


bool utils::isNumber(const std::string &s) {    
  return !s.empty() && s.find_first_not_of("0123456789") == std::string::npos;
}

bool utils::endsWith(const std::string &s, const std::string &c) {
  return (s.substr(s.size() - c.size(), s.size()) == c);
}

bool utils::startsWith(const std::string &s, const std::string &c) {
  return (s.substr(0, c.size()) == c);
}
