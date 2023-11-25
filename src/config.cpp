#include "../include/config.hpp"

Config::Config() {
    this->port = 8080;
    this->root = "./html";
    this->serverNamesList.push_back("localhost");
    this->bodySizeLimit = 1024*1024;
}

Config::~Config(){}


Config::Config(Config const &obj){
	*this = obj;
}

Config &Config::operator=(Config const &obj){
	if(this != &obj){
		this->port = obj.port;
		this->root = obj.root;
		this->serverNamesList = obj.serverNamesList;
		this->bodySizeLimit = obj.bodySizeLimit;
		this->locationList = obj.locationList;
	}
	return (*this);
}	

void Config::clear() {
    this->port = 8080;
    this->root = "./html";
    this->serverNamesList.clear();
    this->serverNamesList.push_back("localhost");
    this->bodySizeLimit = 1024*1024;
    this->locationList.clear();
    this->error.clear();
}

bool Config::isValidCgiRequest(std::string requestPath) {
    for(size_t i = 0; i < locationList.size(); i++) {
        if (locationList[i].path == requestPath) {
            if (locationList[i].cgi == "" || locationList[i].cgiPass == "")
                continue;
            std::string fileExtension = locationList[i].cgi.substr(locationList[i].cgi.find_last_of("."));
            if(locationList[i].cgiPass == fileExtension)
                return true && fileExtension == ".py";
            return false;
        }
    }
    return false;
}

std::string Config::getCgiFile(std::string requestPath) {
    for(size_t i = 0; i < locationList.size(); i++) {
        if (locationList[i].path == requestPath)
            return locationList[i].cgi;
    }
    return "";
}

bool	Config::isRedirection(std::string requestPath) {
    for(size_t i = 0; i < locationList.size(); i++) {
        if (locationList[i].path == requestPath) {
            if (locationList[i].redirect == "")
                continue;
            else
                return true;
        }
    }
    return false;
}

std::string Config::getRedirection(std::string requestPath) {
    for(size_t i = 0; i < locationList.size(); i++) {
        if (locationList[i].path == requestPath)
            return locationList[i].redirect;
    }
    return "";
}
