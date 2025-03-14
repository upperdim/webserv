/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 19:11:36 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/14 11:40:36 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <sstream>
#include "EventManager.hpp"
#include "IEventHandler.hpp"
#include "Request.hpp"
#include "Router.hpp"
#include "AHandler.hpp"
#include "Response.hpp"

class Connection : public IEventHandler
{
public:
	Request		request;
	Response	response;

	Connection(const int _cli_socket);
	~Connection();

	bool	isDone() const;

	virtual void handleReadEvent(EventManager& event_manager);
	virtual void handleWriteEvent(EventManager& event_manager);
	virtual void handleErrorEvent(EventManager& event_manager);
	virtual void handleDisConnectEvent(EventManager& event_manager);

private:
	Connection();
	Connection(const Connection& other);
	Connection&	operator=(const Connection& rhs);

	bool	m_done;
};

#endif
