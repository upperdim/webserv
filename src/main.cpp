/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 11:44:18 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/30 16:09:17 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "Config.hpp"
#include "Parser.hpp"
#include "ServerEngine.hpp"
#include "Log.hpp"

Config parseConfig(std::string configFilePath, char *argv0)
{
	Config config;

	try {
		Parser	parser(configFilePath, argv0);
		config = parser.parse();
		config.printConfigs();
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

	std::signal(SIGINT,  handleAbort);
	std::signal(SIGQUIT, handleAbort);

	try {
		ServerEngine serverEngine(config);
		serverEngine.run();
	} catch(const std::exception& e) {
		LOGT(Log::ERROR, "Critical Exception caught ::");
		LOGT(Log::ERROR, e.what());
	}

	std::signal(SIGINT,  SIG_DFL);
	std::signal(SIGQUIT, SIG_DFL);

	return 0;
}
