/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientConnection.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tunsal <tunsal@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 02:25:08 by tunsal            #+#    #+#             */
/*   Updated: 2025/07/02 00:59:01 by tunsal           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientConnection.hpp"
#include "RequestParser.hpp"
#include "Log.hpp"

ClientConnection::ClientConnection(int fd, Server& connectedServer) 
	:	fd(fd),
		connectedServer(connectedServer),
		connectionError(false),
		request(connectedServer.getServerBlockRef())
{
}

void ClientConnection::receiveRequest()
{
	LOG("Reading from ClientConnection fd = " << fd);
	char buffer[RECV_BUFFER_SIZE];
	ssize_t bytesRead = recv(fd, &buffer, RECV_BUFFER_SIZE - 1, 0);
	LOG(bytesRead << " bytes read");

	if (bytesRead > 0) {
		request.rawRequest.append(buffer, bytesRead);
		LOG("rawRequest.length() = " << request.rawRequest.length());
		buffer[bytesRead] = '\0';
		LOG("buffer = <<<" << buffer << ">>>");
		RequestParser::parseNext(request);
	} else if (bytesRead < 0) {
		connectionError = true;
		LOGT(Log::ERROR, "ClientConnection error, fd = " << fd);
	} else if (bytesRead == 0) {
		request.doneReceiving = true;
		LOG("ClientConnection fd = " << fd << " has received all of the request");
	}
}

void ClientConnection::sendResponse()
{
	LOG("Writing to ClientConnection fd = " << fd);
	
	std::string	chunk = response.getNextChunk();
	send(fd, chunk.c_str(), chunk.length(), 0); // TODO: return val & checks
}

int         ClientConnection::getFd()                 { return fd; }
bool        ClientConnection::getConnectionError()    { return connectionError; }
Server&     ClientConnection::getConnectedServer()    { return connectedServer; }
Request&    ClientConnection::getRequest()            { return request; }
Response&   ClientConnection::getResponse()           { return response; }
// void        ClientConnection::setResponse(Response r) { response = r; } // TODO: disabled for now due to = overload
