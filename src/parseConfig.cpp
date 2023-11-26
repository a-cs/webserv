#include "../include/parseConfig.hpp"

ParseConfig::ParseConfig(std::string file) {
	std::string fileContent;
	std::list<std::string> lines;

	fileContent = readFile(file);
	if (this->error.onError)
		return;
	lines = parseFile(fileContent);
	getConfig(lines);
}

ParseConfig::~ParseConfig() {}

std::string ParseConfig::readFile(std::string file) {
	this->fd.open(file.c_str());
	if (!this->fd.is_open()) {
		this->error.msg = "cannot open file";
		this->error.onError = true;
		return "";
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
			addLocationProperties(line, &loc);
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
				addLocationProperties(lines.front(), &loc);
				lines.pop_front();
			}
			if (loc.allowedMethods.size() == 0) {
				loc.allowedMethods.push_back("GET");
				loc.allowedMethods.push_back("POST");
				loc.allowedMethods.push_back("DELETE");
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
			this->configList.push_back(config);
			config.clear();
			continue;
		}
		addConfigProperties(line, &config);
	}
	if (onServer)
		this->error.onError = true;
	if (this->error.onError)
		this->error.msg = " invalid sintax on config file";
}

void ParseConfig::addConfigProperties(std::string line, Config *config) {
	std::vector<std::string> tokens;

	tokens = utils::split(line, " ");
	if (tokens.size() == 1) {
		this->error.onError = true;
		return;
	}
	if (tokens[0] == "listen" && tokens.size() == 2) {
		int port = std::atoi(tokens[1].c_str());
		if (port < 1024 || port > 49151) {
			this->error.onError = true;
			return;
		}
		config->port = port;
	} else if (tokens[0] == "listen" && tokens.size() != 2) {
		this->error.onError = true;
		return;
	} else if (tokens[0] == "root" && tokens.size() == 2) {
		config->root = tokens[1];
	} else if (tokens[0] == "root" && tokens.size() != 2) {
		this->error.onError = true;
		return;
	} else if (tokens[0] == "server_name" && tokens.size() >= 2) {
		for (size_t i = 1; i < tokens.size(); i++) {
			if (std::find(config->serverNamesList.begin(), config->serverNamesList.end(), tokens[i]) == config->serverNamesList.end())
				config->serverNamesList.push_back(tokens[i]);
		}
	} else if (tokens[0] == "server_name" && tokens.size() < 2) {
		this->error.onError = true;
		return;
	} else if (tokens[0] == "client_max_body_size" && tokens.size() == 2) {
		std::stringstream ss(tokens[1]);
		ss >> config->bodySizeLimit;
	} else if (tokens[0] == "client_max_body_size" && tokens.size() != 2) {
		this->error.onError = true;
		return;
	} else if (tokens[0] == "error_page" && tokens.size() == 3) {
		std::stringstream ss(tokens[1]);
		int code;
		ss >> code;
		if (code < 400) {
			this->error.onError = true;
			return;
		}
		config->errorPageList[code] = tokens[2];
	} else if (tokens[0] == "error_page" && tokens.size() != 3) {
		this->error.onError = true;
		return;
	} else {
		this->error.onError = true;
	}
}

void ParseConfig::addLocationProperties(std::string line, Location *loc) {
	std::vector<std::string> tokens;

	tokens = utils::split(line, " ");
	if (tokens.size() == 1) {
		this->error.onError = true;
		return;
	}
	if (tokens[0] == "location" && tokens.size() == 2) {
		loc->path = tokens[1];
	} else if (tokens[0] == "location" && tokens.size() != 2) {
		this->error.onError = true;
		return;
	} else if (tokens[0] == "http_methods" && tokens.size() >= 2) {
		for (size_t i = 1; i < tokens.size(); i++) {
			if (tokens[i] != "GET" && tokens[i] != "POST" && tokens[i] != "DELETE") {
				this->error.onError = true;
				return;
			}
			loc->allowedMethods.push_back(tokens[i]);
		}
	} else if (tokens[0] == "http_methods" && tokens.size() != 2) {
		this->error.onError = true;
		return;
	} else if (tokens[0] == "index" && tokens.size() >= 2) {
		for (size_t i = 1; i < tokens.size(); i++) {
			if (std::find(loc->indexList.begin(), loc->indexList.end(), tokens[i]) == loc->indexList.end())
				loc->indexList.push_back(tokens[i]);
		}
	} else if (tokens[0] == "index" && tokens.size() < 2) {
		this->error.onError = true;
		return;
	} else if (tokens[0] == "cgi_pass" && tokens.size() == 2) {
		loc->cgiPass = tokens[1];
	} else if (tokens[0] == "cgi_pass" && tokens.size() != 2) {
		this->error.onError = true;
		return;
	} else if (tokens[0] == "cgi" && tokens.size() == 2) {
		loc->cgi = tokens[1];
	} else if (tokens[0] == "cgi" && tokens.size() != 2) {
		this->error.onError = true;
		return;
	} else if (tokens[0] == "directory_listing" && tokens.size() == 2) {
		if (tokens[1] == "on")
			loc->isDirectoryEnable = true;
		else if (tokens[1] == "off")
			loc->isDirectoryEnable = false;
		else {
			this->error.onError = true;
			return;
		}
	} else if (tokens[0] == "directory_listing" && tokens.size() != 2) {
		this->error.onError = true;
		return;
	} else if (tokens[0] == "redirection" && tokens.size() == 2) {
		loc->redirect = tokens[1];
	} else if (tokens[0] == "redirection" && tokens.size() != 2) {
		this->error.onError = true;
		return;
	} else if (tokens[0] == "upload_path" && tokens.size() == 2) {
		loc->uploadPath = tokens[1];
	} else if (tokens[0] == "upload_path" && tokens.size() != 2) {
		this->error.onError = true;
		return;
	} else {
		this->error.onError = true;
		return;
	}
}
