/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientConnection.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tunsal <tunsal@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 02:25:13 by tunsal            #+#    #+#             */
/*   Updated: 2025/06/29 17:21:32 by tunsal           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTCONNECTION_HPP
#define CLIENTCONNECTION_HPP

#include <string>
#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"

#define RECV_BUFFER_SIZE	2048

class ClientConnection
{
private:
	int			fd;
	Server&		connectedServer;
	bool		connectionError;
	Request		request;
	Response	response;

public:
	ClientConnection(int fd, Server& connectedServer);

	void		receiveRequest();
	void		sendResponse();
	
	int			getFd();
	bool		getConnectionError();
	Server&		getConnectedServer();
	Request&	getRequest();
	Response&	getResponse();
};

#endif
