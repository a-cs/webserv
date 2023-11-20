#include "../include/config.hpp"

Config::Config() {
    this->port = 8080;
    this->root = "./html";
    this->serverNamesList.push_back("localhost");
    this->bodySizeLimit = 1024*1024;
}

Config::~Config(){}

void Config::clear() {
    this->port = 8080;
    this->root = "./html";
    this->serverNamesList.clear();
    this->serverNamesList.push_back("localhost");
    this->bodySizeLimit = 1024*1024;
    this->locationList.clear();
    this->error.clear();
}
