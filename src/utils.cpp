#include "utils.hpp"

std::vector<std::string> utils::split(std::string s, std::string delimiter) {
	size_t start = 0, end, delimLen = delimiter.length();
	std::string token;
	std::vector<std::string> tokenList;

	while ((end = s.find(delimiter, start)) != std::string::npos) {
		token = s.substr(start, end - start);
		start = end + delimLen;
		tokenList.push_back(token);
	}

	tokenList.push_back(s.substr(start));
	return tokenList;
}

static std::string &rtrim(std::string &s, const char *ws) {
	s.erase(s.find_last_not_of(ws) + 1);
	return s;
}

static std::string &ltrim(std::string &s, const char *ws) {
	s.erase(0, s.find_first_not_of(ws));
	return s;
}

std::string utils::trim(std::string s) {
	const char *ws = " \t\n\r\f\v";
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

bool utils::isDirectory(const std::string &path) {
	struct stat buf;
	if (stat(path.c_str(), &buf) == -1)
		return false;
	return S_ISDIR(buf.st_mode);
}

bool utils::isFile(const std::string &s) {
	struct stat buf;
	if (stat(s.c_str(), &buf) == -1)
		return false;
	return S_ISREG(buf.st_mode);
}

bool utils::pathExists(std::string path) {
	struct stat buf;
	return stat(path.c_str(), &buf) == 0;
}

std::string utils::getFile(std::string path) {
	std::ifstream ifs;
	std::stringstream buf;

	ifs.open(path.c_str());
	buf << ifs.rdbuf();
	ifs.close();
	return buf.str();
}

std::string utils::formatSize(size_t size) {
	std::stringstream ss;

	const char *units[] = {"B", "KB", "MB", "GB"};
	int unit = 0;
	while (size > 1024 && unit < 3) {
		size /= 1024;
		++unit;
	}
	ss << size << units[unit];
	return ss.str();
}
