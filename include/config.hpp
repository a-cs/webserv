#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>
#include "location.hpp"
#include "error.hpp"
#include <map>

class Config {
	public:
		int							port;
		Error						error;
		std::string					root;
		long double					bodySizeLimit;
		std::vector<Location>		locationList;
		std::vector<std::string>	serverNamesList;
		std::map<int, std::string>	errorPageList;

		Config();
		~Config();
		Config(Config const &obj);
		Config &operator=(Config const &obj);

		void		clear();
		bool		isRedirection(std::string requestPath);
		bool		isValidCgiRequest(std::string requestPath);
		std::string	getCgiFile(std::string requestPath);
		std::string	getRedirection(std::string requestPath);
};

#endif
