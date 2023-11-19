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
    for (size_t i = 0; i < this->configList.size(); i++) {
        std::cout << configList[i].port << std::endl;
        std::cout << configList[i].root << std::endl;
        for (size_t j = 0; j < this->configList[i].serverNamesList.size(); j++) {
            std::cout << configList[i].serverNamesList[j] << std::endl;
        }
        std::cout << configList[i].bodySizeLimit << std::endl;
    }
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

void ParseConfig::getConfig(std::list<std::string> lines) {
    bool onServer = false;
    Config config;

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
            Location loc;
            // std::cout << "location first line: " << line << std::endl;
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
                // std::cout << "location: " << lines.front() << std::endl;
                lines.pop_front();
            }
            config.locationList.push_back(loc);
            continue;
        }
        if (line == "{") {
            this->error.onError = true;
            break;
        }
        if (onServer && line == "}") {
            onServer = false;
            this->configList.push_back(config); //todo: checar se funciona sem copia
            config.clear();
            continue;
        }
        addConfigProperties(line, &config);
        // std::cout << line << std::endl;
    }
    if (onServer)
        this->error.onError = true;
    // if (this->error.onError) {
    //     this->error.msg = " invalid sintax on config file";
    // }
}

void ParseConfig::addConfigProperties(std::string line, Config *config) {
    std::vector<std::string> tokens;

    tokens = utils::split(line, " ");
    if (tokens.size() == 1) {
        this->error.onError = true;
        this->error.msg = " split falhou";
        return;
    }
    if (tokens[0] == "listen" && tokens.size() == 2) { //todo: verificar se aceita varias portas
        int port = std::atoi(tokens[1].c_str());
        if (port < 1024 || port > 49151) {
           this->error.onError = true;
           this->error.msg = " range port falhou";
            return; 
        }
        config->port = port;
    } else if (tokens[0] == "listen" && tokens.size() != 2) {
        this->error.onError = true;
        this->error.msg = " listen falhou";
        return;
    } else if (tokens[0] == "root" && tokens.size() == 2) {
        config->root = tokens[1];
    } else if (tokens[0] == "root" && tokens.size() != 2) {
        this->error.onError = true;
        this->error.msg = " root falhou";
        return;
    } else if (tokens[0] == "server_name" && tokens.size() >= 2) {
        for (size_t i = 1; i < tokens.size(); i++) {
            if (std::find(config->serverNamesList.begin(), config->serverNamesList.end(), tokens[i]) == config->serverNamesList.end())
                config->serverNamesList.push_back(tokens[i]);
        }
    } else if (tokens[0] == "server_name" && tokens.size() < 2) {
        this->error.onError = true;
        this->error.msg = " server_name falhou";
        return;
    } else if (tokens[0] == "client_max_body_size" && tokens.size() == 2) {
        config->bodySizeLimit = std::atoi(tokens[1].c_str()); //todo: alterar atoi
    } else if (tokens[0] == "client_max_body_size" && tokens.size() != 2) {
        this->error.onError = true;
        this->error.msg = " client_max_body_size falhou";
        return;
    } else {
        this->error.onError = true;
        this->error.msg = " token invalido";
    }
}

// void ParseConfig::addLocationProperties(std::string line, Location *loc) {

// }
