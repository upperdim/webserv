/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventManager.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 19:30:12 by nmihaile          #+#    #+#             */
/*   Updated: 2025/02/22 19:30:18 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EventManager.hpp"

EventManager::EventManager(bool& _running)
	:	m_running(_running)
{
}

EventManager::~EventManager()
{
}


/* ************************************************************************** */
/* ************************************************************************** */


void	EventManager::run(void)
{
	if (m_running == false)
		return ;

	Log::msg("running Webserv: ", "serving the world wide web", LIGHTGREEN, GREEN);
	while (m_running)
	{
		Log::info("polling");
		int	num_fd = poll(m_pollfds.data(), m_pollfds.size(), 0);
		(void) num_fd;
		sleep(1);
	}
}

void	EventManager::registerFd(Server* _server)
{
	if (_server == nullptr)
		return ;
	
	auto it = m_fd_to_server.find(_server->srv_socket);
	if (it != m_fd_to_server.end())
		throw ( std::runtime_error("Server socket file descriptor is already registered in the event loop") );
	
	m_pollfds.emplace_back( (pollfd){_server->srv_socket, 0, 0} );
	m_fd_to_server[_server->srv_socket] = _server;
}

void	EventManager::unregisterFd(const int _fd)
{
	(void)_fd;
}

