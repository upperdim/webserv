/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventManager.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 11:39:38 by nmihaile          #+#    #+#             */
/*   Updated: 2025/02/22 19:23:34 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EVENTMANAGER_HPP
#define EVENTMANAGER_HPP

#include <vector>
#include <map>
#include <stdexcept>
#include <poll.h>
#include "Server.hpp"
#include "webserv.h"

// EventLoop class could also be named:
//	-	EventManager
// 	-	WebServ
//	-	ServerHub
//	-	ServerManager
//	-	ServerController

class EventManager
{
public:
	EventManager(bool& _running);
	~EventManager();

	void	run(void);
	void	registerFd(Server* _server);
	void	unregisterFd(const int _fd);

private:
	EventManager();
	EventManager(const EventManager& other);
	EventManager&	operator=(const EventManager& rhs);

	bool&						m_running;
	std::vector<struct pollfd>	m_pollfds;
	std::map<int, Server*>		m_fd_to_server;
};

#endif