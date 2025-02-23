/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventManager.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 11:39:38 by nmihaile          #+#    #+#             */
/*   Updated: 2025/02/23 13:02:40 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EVENTMANAGER_HPP
#define EVENTMANAGER_HPP

#include <vector>
#include <map>
#include <stdexcept>
#include <poll.h>
#include <utility>
#include "IEventHandler.hpp"
#include "Server.hpp"
#include "Connection.hpp"
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

	void	registerFd(IEventHandler* _handler, short events);
	void	unregisterFd(const int _fd);
	void	processPendingRegistrations(void);
	void	handleFdEvents(int max_events);
	void	run(void);

private:
	EventManager();
	EventManager(const EventManager& other);
	EventManager&	operator=(const EventManager& rhs);


	bool&											m_running;
	std::vector<struct pollfd>						m_pollfds;
	std::map<int, IEventHandler*>					m_fd_to_EventHandler;
	std::vector<std::pair<IEventHandler*, short>>	m_register_queue;
};

#endif