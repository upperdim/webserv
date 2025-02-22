/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventLoop.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 11:39:37 by nmihaile          #+#    #+#             */
/*   Updated: 2025/02/22 13:50:38 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EventLoop.hpp"

EventLoop::EventLoop(bool& _running)
	:	m_running(_running)
{
}

EventLoop::~EventLoop()
{
}


/* ************************************************************************** */
/* ************************************************************************** */


void	EventLoop::run(void)
{
	int	num_fd;

	if (m_running == false)
		return ;

	std::cout	<< LIGHTGREEN
				<< BOLD << "running Webserv: "
				<< REGULAR << GREEN << "serving the world wide web"
				<< RESET << std::endl;
	while (m_running)
	{
		Log::info("polling");
		num_fd = poll(m_pollfds.data(), m_pollfds.size(), 0);
		sleep(1);
	}
}

void	EventLoop::registerFd(Server* _server)
{
	if (_server == nullptr)
		return ;
	
	auto it = m_fd_to_server.find(_server->srv_socket);
	if (it != m_fd_to_server.end())
		throw ( std::runtime_error("Server socket file descriptor is already registered in the event loop") );
	
	m_pollfds.emplace_back( (pollfd){_server->srv_socket, 0, 0} );
	m_fd_to_server[_server->srv_socket] = _server;
}

void	EventLoop::unregisterFd(const int _fd)
{
	(void)_fd;
}

