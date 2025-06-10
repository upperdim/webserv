/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 11:44:18 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/10 19:40:41 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "Config.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "EventManager.hpp"
#include "Server.hpp"
#include "Log.hpp"

bool	g_running = true;

void	terminate(std::string msg, int exit_code)
{
	if (exit_code == 0)
		Log::info(msg);
	else
		Log::error(msg);
	exit(exit_code);
}

int	main(int argc, char **argv)
{
	std::string configFilePath;

	if (argc > 2) {
		std::cout << "Usage: ./webserv [configuration file]" << std::endl;
		return 0;
	} else if (argc == 2) {
		configFilePath = argv[1];
	} else {
		configFilePath = "default.conf"; // default config path
	}

	std::signal(SIGINT, handleAbort);
	std::signal(SIGQUIT, handleAbort);

	Config config;

	try	{
		Lexer	lexer(readFile(configFilePath));
		Parser	parser(lexer);

		config = parser.parse();
		// config = parser.mockParseConfig(configFilePath);
		
	} catch (const std::exception& e)	{
		terminate(e.what(), 1);
	}

	try {
		EventManager event_manager(g_running);
		Server* server = new Server();

		event_manager.registerFd(server, POLLIN | POLLERR | POLL_HUP);
		event_manager.processPendingRegistrations();

		event_manager.run();
	} catch(const std::exception& e) {
		LOG_ERROR_LM("Critical Exception caught","::");
		LOG_ERROR(e.what());
		g_running = false;
	}

	std::signal(SIGINT, SIG_DFL);
	std::signal(SIGQUIT, SIG_DFL);

	return (0);
}
