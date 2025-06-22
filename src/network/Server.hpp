/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 11:42:37 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/22 10:25:48 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>			//	getaddrinfo()
#include <fcntl.h>			// Nonblocking -> for Port immideate realod
#include <string>
#include "webserv.hpp"
#include "EventManager.hpp"
#include "IEventHandler.hpp"
#include "Connection.hpp"
#include "Config.hpp"
#include "Log.hpp"

class Server : public IEventHandler
{
public:
	Server(ServerBlock serverBlock);
	~Server();

	void	accept_connection(EventManager& event_manager);

	virtual void handleReadEvent(EventManager& event_manager);
	virtual void handleWriteEvent(EventManager& event_manager);
	virtual void handleErrorEvent(EventManager& event_manager);
	virtual void handleDisConnectEvent(EventManager& event_manager);

private:
	struct sockaddr_in	m_srvAddr;
	ServerBlock			m_serverBlock;

	Server(const Server& other);
	Server&	operator=(const Server& rhs);
};

#endif
