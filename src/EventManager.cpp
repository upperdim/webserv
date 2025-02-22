/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventManager.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 19:30:12 by nmihaile          #+#    #+#             */
/*   Updated: 2025/02/22 20:49:38 by nmihaile         ###   ########.fr       */
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
		Log::info("polling fds");
		int	fd_count = poll(m_pollfds.data(), m_pollfds.size(), 1000);	// TODO: is 1000 a good value for timeout
		(void) fd_count;
	}
}

void	EventManager::registerFd(IEventHandler* _handler)
{
	if (_handler == nullptr)
		return ;
	
	auto it = m_fd_to_EventHandler.find(_handler->socket_fd);
	if (it != m_fd_to_EventHandler.end())
		throw ( std::runtime_error("Server socket file descriptor is already registered in the event loop") );
	
	m_pollfds.emplace_back( (pollfd){_handler->socket_fd, 0, 0} );
	m_fd_to_EventHandler[_handler->socket_fd] = _handler;
}

void	EventManager::unregisterFd(const int _fd)
{
	// TODO: still not implemented
	(void)_fd;
}

