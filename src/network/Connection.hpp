/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 19:11:36 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/26 16:58:47 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <sstream>
#include <filesystem>
#include "Config.hpp"
#include "EventManager.hpp"
#include "IEventHandler.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Utils.hpp"

class Connection : public IEventHandler
{
public:
	Request		request;
	Response	response;

	Connection(const int _cli_socket, const ServerBlock& _serverBlock);
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

	Response	handleGetRequest(const Request& request);
	std::string	fetchErrorPage(int _status_code) const;
	void		createErrorResponse(Response& response, int _status_code);
	Response	handleErrorRequest(const Request& request);
	Response	handlePostRequest(const Request& request);
	Response	handleDeleteRequest(const Request& request);

	bool				m_done;
	const ServerBlock&	m_serverBlock;
};

#endif
