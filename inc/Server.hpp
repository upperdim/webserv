/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 11:42:37 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/18 16:24:16 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include "webserv.hpp"
#include "EventManager.hpp"
#include "IEventHandler.hpp"
#include "Connection.hpp"

class Server : public IEventHandler
{
public:
	Server();
	~Server();

	void	accept_connection(EventManager& event_manager);

	virtual void handleReadEvent(EventManager& event_manager);
	virtual void handleWriteEvent(EventManager& event_manager);
	virtual void handleErrorEvent(EventManager& event_manager);
	virtual void handleDisConnectEvent(EventManager& event_manager);

	void				setRootDir(const std::string& _root_dir);
	const std::string	getRootDir(void) const;

private:
	struct sockaddr_in	m_srv_addr;
	std::string			m_root_dir;

	Server(const Server& other);
	Server&	operator=(const Server& rhs);
};

#endif
