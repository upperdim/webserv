/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventLoop.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 11:39:38 by nmihaile          #+#    #+#             */
/*   Updated: 2025/02/22 12:50:33 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EVENTLOOP_HPP
#define EVENTLOOP_HPP

#include <vector>
#include <map>
#include <stdexcept>
#include <poll.h>
#include "Server.hpp"
#include "webserv.h"

class EventLoop
{
public:
	EventLoop(bool& _running);
	~EventLoop();

	void	run(void);
	void	registerFd(Server* _server);
	void	unregisterFd(const int _fd);

private:
	EventLoop();
	EventLoop(const EventLoop& other);
	EventLoop&	operator=(const EventLoop& rhs);

	bool&						m_running;
	std::vector<struct pollfd>	m_pollfds;
	std::map<int, Server*>		m_fd_to_server;
};

#endif