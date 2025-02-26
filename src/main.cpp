/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 11:44:18 by nmihaile          #+#    #+#             */
/*   Updated: 2025/02/26 13:33:01 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.h"
#include "EventManager.hpp"
#include "Server.hpp"

bool	g_running = true;

int	main(void)
{
	std::signal(SIGINT, handleAbort);
	std::signal(SIGQUIT, handleAbort);

	// Request rq;
	// Log::debug("State: ", static_cast<size_t>(rq.state));

	try
	{
		EventManager event_manager(g_running);
		Server* server = new Server();

		event_manager.registerFd(server, POLLIN | POLLERR | POLL_HUP);
		event_manager.processPendingRegistrations();

		event_manager.run();
	}
	catch(const std::exception& e)
	{
		Log::error("Critical Exception caught","::");
		Log::error(e.what());
		g_running = false;
	}

	std::signal(SIGINT, SIG_DFL);
	std::signal(SIGQUIT, SIG_DFL);

	return (0);
}
