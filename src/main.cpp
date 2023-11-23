#include "../include/server.hpp"
#include "../include/parseConfig.hpp"

int run(std::vector<Config>  configList);

int main(int argc, char** argv) {
	if (argc > 2) {
		std::cout << "Error: invalid arguments, you must provide only one file name as an argument.\n";
		return 1;
	}
	std::string file = argc == 2 ? argv[1] : "./config/default.conf";
	ParseConfig parse(file);
	if (parse.error.onError) {
		std::cout << "Error: " << parse.error.msg << std::endl;
		return 1;
	}
	std::vector<Config> configList = parse.configList;
	return run(configList);
}
