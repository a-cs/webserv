#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>
#include "location.hpp"
#include "error.hpp"
#include <map>

class Config
{
	private:
	public:
		Config();
		~Config();
		Config(Config const &obj);
		Config &operator=(Config const &obj);
		void clear();
		bool isValidCgiRequest(std::string requestPath);
		std::string getCgiFile(std::string requestPath);
		bool	isRedirection(std::string requestPath);
		std::string getRedirection(std::string requestPath);

        int port;
        std::string root;
        std::vector<std::string> serverNamesList;
        long double bodySizeLimit;
        std::vector<Location> locationList;
        Error error;
		std::map<int, std::string> errorPageList;
};

#endif
