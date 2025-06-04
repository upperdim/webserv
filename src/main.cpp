/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 11:44:18 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/04 19:25:23 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "Parser.hpp"
#include "EventManager.hpp"
#include "Server.hpp"
#include "Log.hpp"

bool	g_running = true;

void	terminate(std::string msg, int exit_code)
{
	Log::info(msg);
	exit(exit_code);
}

int	main(int ac, char **av)
{
	if (ac != 2)
		terminate("usage: ./webserv config.conf", 0);

	std::signal(SIGINT, handleAbort);
	std::signal(SIGQUIT, handleAbort);

	try
	{
		Lexer	lexer(readFile(av[1]));
		Parser	parser(lexer);
		std::string		config;	// TODO: change to config struct

		try
		{
			config = parser.parse();
		}
		catch (const std::exception& e)
		{
			terminate(e.what(), 1);
		}

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
