/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 11:44:18 by nmihaile          #+#    #+#             */
/*   Updated: 2025/02/22 14:29:21 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.h"

bool	g_running = true;

int	main(void)
{

	std::signal(SIGINT, handleAbort);
	std::signal(SIGQUIT, handleAbort);

	EventLoop el(g_running);

	Server server;

	try
	{
		el.registerFd(&server);
	}
	catch(const std::exception& e)
	{
		Log::error(e.what());
		g_running = false;
	}
	
	el.run();

	el.unregisterFd(server.srv_socket);

	std::signal(SIGINT, SIG_DFL);
	std::signal(SIGQUIT, SIG_DFL);

	return (0);
}
