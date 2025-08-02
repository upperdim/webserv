#include <csignal>
#include "Config.hpp"
#include "Parser.hpp"
#include "ServerEngine.hpp"
#include "Log.hpp"

void handleAbortSignal(int signum)
{
	(void) signum;

	ServerEngine::isRunning = false;

	std::cout << std::endl;
	std::cout.flush();
}

Config parseConfig(std::string configFilePath, char *programName)
{
	Config config;

	try {
		Parser	parser(configFilePath, programName);
		config = parser.parse();
		// config.printConfigs();
	} catch (std::exception& e) {
		LOGT(Log::ERROR, e.what());
		exit(EXIT_FAILURE);
	}

	return config;
}

std::string handleArgs(int argc, char **argv)
{
	std::string configFilePath;

	if (argc > 2) {
		std::cout << "Usage: ./webserv [configuration file]" << std::endl;
		return 0;
	} else if (argc == 2) {
		configFilePath = argv[1];
	} else {
		configFilePath = "default.conf";
	}

	return configFilePath;
}

int	main(int argc, char **argv)
{
	std::string configFilePath = handleArgs(argc, argv);
	Config config = parseConfig(configFilePath, argv[0]);

	std::signal(SIGINT,  handleAbortSignal);
	std::signal(SIGQUIT, handleAbortSignal);
	std::signal(SIGPIPE, SIG_IGN);

	try {
		ServerEngine serverEngine(config);
		serverEngine.run();
	} catch(const std::exception& e) {
		LOGT(Log::ERROR, "Critical Exception caught ::");
		LOGT(Log::ERROR, e.what());
	}

	std::signal(SIGINT,  SIG_DFL);
	std::signal(SIGQUIT, SIG_DFL);
	std::signal(SIGPIPE, SIG_DFL);

	return 0;
}
