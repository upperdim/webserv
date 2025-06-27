/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventManager.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 11:39:38 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/13 18:35:20 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EVENTMANAGER_HPP
#define EVENTMANAGER_HPP

#include <vector>
#include <map>
#include <set>
#include <stdexcept>
#include <poll.h>
#include <utility>
#include "webserv.hpp"
#include "Server.hpp"
#include "Connection.hpp"

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

	void	registerFd(IEventHandler* _handler, const short events);
	void	unregisterFd(const int _fd);
	void	setFdEvents(const int _fd, const short _events);
	void	processPendingRegistrations(void);
	void	processClosingConnections(void);

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
	std::set<int>									m_unregister_queue;

	void	cleanupFinishedConnections(void);
	void	closeAllServers(void);
	void	closeAllConnections(void);
	void	delete_pollfd(const int _fd);
};

#endif