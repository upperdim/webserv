/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventManager.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 19:30:12 by nmihaile          #+#    #+#             */
/*   Updated: 2025/02/25 14:48:58 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EventManager.hpp"

EventManager::EventManager(bool& _running)
	:	m_running(_running)
{
}

EventManager::~EventManager()
{
	closeAllServers();
	closeAllConnections();
	processClosingConnections();
}


/* ************************************************************************** */
/* ************************************************************************** */


void	EventManager::registerFd(IEventHandler* _handler, const short events)
{
	if (_handler == nullptr)
		return ;
	
	auto it = m_fd_to_EventHandler.find(_handler->socket_fd);
	if (it != m_fd_to_EventHandler.end())
		throw ( std::runtime_error("socket_fd is already registered in the event manager") );

	m_register_queue.push_back( std::pair<IEventHandler*, short>(_handler, events) );
}

void	EventManager::setFdEvents(const int _fd, const short _events)
{
	for (auto it = m_pollfds.begin(); it < m_pollfds.end(); ++it)
		if (it->fd == _fd)
		{
			it->events = _events;
			break ;
		}
}

void	EventManager::unregisterFd(const int _fd)
{
	m_unregister_queue.insert(_fd);
}

void	EventManager::processPendingRegistrations(void)
{
	if (m_register_queue.size() == 0)
		return ;

	for (std::vector<std::pair<IEventHandler*, short>>::iterator it = m_register_queue.begin(); it < m_register_queue.end(); ++it)
	{
		auto found = m_fd_to_EventHandler.find(it->first->socket_fd);
		if (found != m_fd_to_EventHandler.end())
		{
			Log::error("Omitting EventHandler with fd: " + std::to_string(it->first->socket_fd));
			continue ;
		}
		
		m_pollfds.emplace_back( (pollfd){it->first->socket_fd, it->second, 0} );
		m_fd_to_EventHandler[it->first->socket_fd] = it->first;
	}
	
	m_register_queue.clear();
}

void	EventManager::processClosingConnections(void)
{
	if (m_unregister_queue.empty())
		return ;

	// remove queued Connections
	for (auto it = m_unregister_queue.begin(); it != m_unregister_queue.end(); ++it)
	{
		std::map<int, IEventHandler*>::iterator found = m_fd_to_EventHandler.find(*it);
		if (found != m_fd_to_EventHandler.end())
		{
			delete(found->second);
			m_fd_to_EventHandler.erase(*it);
			delete_pollfd(*it);

			Log::error("CLOSED", std::string("Connection fd: ") + std::to_string(*it));
		}
	}

	m_unregister_queue.clear();
}

void	EventManager::handleFdEvents(int max_events)
{
	int counted = 0;

	for (auto it = m_pollfds.begin(); it < m_pollfds.end(); ++it)
	{
		bool count_me = false;

		if (it->revents & POLLERR)
		{
			count_me = true;
			m_fd_to_EventHandler[it->fd]->handleErrorEvent(*this);
		}
		if (it->revents & POLLHUP)
		{
			count_me = true;
			m_fd_to_EventHandler[it->fd]->handleDisConnectEvent(*this);
		}

		if (it->revents & POLLOUT)
		{
			count_me = true;
			m_fd_to_EventHandler[it->fd]->handleWriteEvent(*this);
		}
		if (it->revents & POLLIN)
		{
			count_me = true;
			m_fd_to_EventHandler[it->fd]->handleReadEvent(*this);
		}
		
		if (count_me)
			++counted;
		if (counted >= max_events)
			break ;
		(void)max_events;
	}
}

void	EventManager::run(void)
{
	if (m_running == false)
		return ;

	Log::msg("running Webserv: ", "serving the world wide web", LIGHTGREEN, GREEN);
	while (m_running)
	{
		// Log::info("polling fds");
		int	fd_count = poll(m_pollfds.data(), m_pollfds.size(), 3000);	// TODO: is 1000 a good value for timeout
		if (fd_count < 0 && g_running)
			throw ( std::runtime_error("an error occured while polling") );

		handleFdEvents(fd_count);

		processPendingRegistrations();

		cleanupFinishedConnections();
		processClosingConnections();
	}
}


/* ************************************************************************** */
/* ************************************************************************** */


void	EventManager::cleanupFinishedConnections(void)
{
	if (m_pollfds.size() == 0)
		return ;

	// find Connections FDs to clear and queue them
	for (std::vector<struct pollfd>::iterator it = m_pollfds.begin(); it < m_pollfds.end(); ++it)
	{
		auto found = m_fd_to_EventHandler.find(it->fd);
		auto* conn = dynamic_cast<Connection*>(found->second);
		if (found == m_fd_to_EventHandler.end() || conn == nullptr || conn->isDone() == false)
			continue ;
		unregisterFd(it->fd);
	}
}

void	EventManager::closeAllServers(void)
{
	if (m_pollfds.size() == 0)
		return ;

	// find Connections FDs and unregisterFd() them
	for (std::vector<struct pollfd>::iterator it = m_pollfds.begin(); it < m_pollfds.end(); ++it)
	{
		auto found = m_fd_to_EventHandler.find(it->fd);
		auto* srv = dynamic_cast<Server*>(found->second);
		if (found != m_fd_to_EventHandler.end() && srv != nullptr)
			unregisterFd(it->fd);
	}	
}

void	EventManager::closeAllConnections(void)
{
	if (m_pollfds.size() == 0)
		return ;

	// find Connections FDs and unregisterFd() them
	for (std::vector<struct pollfd>::iterator it = m_pollfds.begin(); it < m_pollfds.end(); ++it)
	{
		auto found = m_fd_to_EventHandler.find(it->fd);
		auto* conn = dynamic_cast<Connection*>(found->second);
		if (found != m_fd_to_EventHandler.end() && conn != nullptr)
			unregisterFd(it->fd);
	}	
}

void	EventManager::delete_pollfd(const int _fd)
{
	for (auto it = m_pollfds.begin(); it < m_pollfds.end(); ++it)
		if (it->fd == _fd)
		{
			m_pollfds.erase(it);
			break ;
		}
}
