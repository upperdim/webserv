/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tunsal <tunsal@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 11:44:18 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/29 17:02:02 by tunsal           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "Config.hpp"
#include "Parser.hpp"
#include "ServerEngine.hpp"
#include "Log.hpp"

bool	g_running = true;

Config parseConfig(std::string configFilePath)
{
	Config config;

	try {
		Parser	parser(configFilePath);
		config = parser.parse();
		config.printConfigs();
	} catch (std::exception& e) {
		Log::error(e.what());
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
	Config config = parseConfig(configFilePath);

	std::signal(SIGINT, handleAbort);
	std::signal(SIGQUIT, handleAbort);

	try {
		ServerEngine serverEngine(config);
		serverEngine.run();
	} catch(const std::exception& e) {
		LOG_ERROR_LM("Critical Exception caught","::");
		LOG_ERROR(e.what());
		g_running = false;
	}

	std::signal(SIGINT, SIG_DFL);
	std::signal(SIGQUIT, SIG_DFL);

	return 0;
}
