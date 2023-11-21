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
