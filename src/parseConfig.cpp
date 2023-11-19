#include "../include/parseConfig.hpp"

ParseConfig::ParseConfig(std::string file) {
    std::string fileContent;
    std::list<std::string> lines;

    fileContent = readFile(file);
    if (this->error.onError)
        return ;
    lines = parseFile(fileContent);
    // for (std::list<std::string>::iterator i = lines.begin(); i != lines.end(); ++i) {
    //     std::cout << *i << std::endl;
    // }
    getConfig(lines);
    this->error.onError = true;
}

ParseConfig::~ParseConfig() {}

std::string ParseConfig::readFile(std::string file) {
    this->fd.open(file.c_str());
    if (!this->fd.is_open()) {
        this->error.msg = "cannot open file";
        this->error.onError = true;
        return NULL;
    }
    std::stringstream buf;
    buf << fd.rdbuf();
    fd.close();

    return buf.str();
}

std::list<std::string> ParseConfig::parseFile(std::string fileContent) {
    std::stringstream ss(fileContent);
    std::list<std::string> lines;
    std::string line;

    while (getline(ss, line)) {
        line = utils::trim(line);
        if (line.size() > 0) {
            if (line.size() > 1 && utils::endsWith(line, "{")) {
                line = line.substr(0, line.size() - 1);
                line = utils::trim(line);
                lines.push_back(line);
                lines.push_back("{");
                continue;
            }
            lines.push_back(line);
        }
    }
    return lines;
}

bool ParseConfig::isServerLine(std::string line) {
    line = utils::trim(line);
    return (line.compare("server {") == 0);
}

bool ParseConfig::isClosedBrackets(std::string line) {
    line = utils::trim(line);
    return (line.compare("}") == 0);
}

bool ParseConfig::isLocationLine(std::string line) {
    std::vector<std::string> locationsTokens;
    line = utils::trim(line);
    locationsTokens = utils::split(line, " ");
    if (locationsTokens.size() == 3 
        && locationsTokens[0].compare("location") == 0
        && locationsTokens[2].compare("{") == 0) {
        return true;
    }
    return false;
}

bool ParseConfig::canRead(bool onServer, bool onLocation, std::string line) {
    if (isServerLine(line) && onServer)
        return false;
    if (isLocationLine(line) && onLocation)
        return false;
    return true;
}

void ParseConfig::getConfig(std::list<std::string> lines) {
    bool onServer = false;

    while (!this->error.onError && !lines.empty()) {
        if (utils::startsWith(lines.front(), "#")) {
            lines.pop_front();
            continue;
        }
        if (!onServer) {
            if (lines.front() != "server") {
                this->error.onError = true;
                break;
            } else {
                lines.pop_front();
                if (lines.front() != "{") {
                    this->error.onError = true;
                    break;
                }
                lines.pop_front();
            }
            onServer = true;
            continue;
        }
        std::string line = lines.front();
        lines.pop_front();
        if (onServer && utils::startsWith(line, "location")) {
            std::cout << "location first line: " << line << std::endl;
            if (lines.front() != "{") {
                this->error.onError = true;
                break;
            }
            lines.pop_front();
            while (!lines.empty()) {
                if (utils::startsWith(lines.front(), "#")) {
                    lines.pop_front();
                    continue;
                }
                if (lines.front() == "}") {
                    lines.pop_front();
                    break;
                }
                if (lines.front() == "{" ||
                    lines.front() == "server" ||
                    utils::startsWith(lines.front(), "location")) {
                    this->error.onError = true;
                    break;
                }
                std::cout << "location: " << lines.front() << std::endl;
                lines.pop_front();
            }
            continue;
        }
        if (line == "{") {
            this->error.onError = true;
            break;
        }
        if (onServer && line == "}") {
            onServer = false;
            std::cout << "server end: " << line << std::endl;
            continue;
        }
        std::cout << line << std::endl;
    }
    if (onServer)
        this->error.onError = true;
    if (this->error.onError) {
        this->error.msg = " invalid sintax on config file";
    }
}

// std::cout << "1 while: " << line << std::endl;
        // if (isServerLine(line)) {
        //     onServer = true;
        //     getline(ss, line);
        //     std::cout << "is server line: " << line << std::endl;
        //     while (!isClosedBrackets(line)) {
        //         if (isLocationLine(line)) {
        //             onLocation = true;
        //             std::cout << line << std::endl;
        //             getline(ss, line);
        //             std::cout << "is location line: " << line << std::endl;
        //             while (!isClosedBrackets(line)) {
        //                 std::cout << "location: " << line << std::endl;
        //                 getline(ss, line);
        //                 if (ss.eof())
        //                     break;
        //             }
        //             if (ss.eof())
        //                 break;
        //             onLocation = false;
        //         }
        //         if (ss.eof())
        //             break;
        //         std::cout << line << std::endl;
        //         getline(ss, line);
        //     }
        //     if (ss.eof())
        //         break;
        //     onServer = false;
        // }