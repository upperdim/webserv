/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 11:44:18 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/13 18:33:42 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "EventManager.hpp"
#include "Server.hpp"
#include "Config.hpp"
#include "Parser.hpp"

bool	g_running = true;

int	main(int argc, char **argv)
{
	std::string configFilePath;

	if (argc > 2) {
		std::cout << "Usage: ./webserv [configuration file]" << std::endl;
		return 0;
	} else if (argc == 2) {
		configFilePath = argv[1];
	} else {
		configFilePath = "config.conf"; // default config path
	}

	std::signal(SIGINT, handleAbort);
	std::signal(SIGQUIT, handleAbort);

	try
	{
		Parser parser;
		Config config = parser.mockParseConfig(configFilePath);

		EventManager event_manager(g_running);
		Server* server = new Server();

		event_manager.registerFd(server, POLLIN | POLLERR | POLL_HUP);
		event_manager.processPendingRegistrations();

		event_manager.run();
	}
	catch(const std::exception& e)
	{
		LOG_ERROR_LM("Critical Exception caught","::");
		LOG_ERROR(e.what());
		g_running = false;
	}

	std::signal(SIGINT, SIG_DFL);
	std::signal(SIGQUIT, SIG_DFL);

	return (0);
}
