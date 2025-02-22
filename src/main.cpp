/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 11:44:18 by nmihaile          #+#    #+#             */
/*   Updated: 2025/02/22 19:28:06 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.h"

bool	g_running = true;

int	main(void)
{

	std::signal(SIGINT, handleAbort);
	std::signal(SIGQUIT, handleAbort);

	EventManager event_manager(g_running);

	Server server;

	try
	{
		event_manager.registerFd(&server);
	}
	catch(const std::exception& e)
	{
		Log::error(e.what());
		g_running = false;
	}
	
	event_manager.run();

	event_manager.unregisterFd(server.srv_socket);

	std::signal(SIGINT, SIG_DFL);
	std::signal(SIGQUIT, SIG_DFL);

	return (0);
}
