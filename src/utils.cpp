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
