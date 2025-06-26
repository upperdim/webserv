/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 19:11:37 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/26 10:38:54 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"
#include "HTTP.hpp"
#include "RequestParser.hpp"

Connection::Connection(const int _cli_socket, const ServerBlock& _serverBlock)
	:	request(_serverBlock),
		m_done(false),
		m_serverBlock(_serverBlock)
{
	socket_fd = _cli_socket;
}

Connection::~Connection()
{
	LOG_WARNING_LM("CLOSING", std::string("Connection fd: ") + std::to_string(socket_fd));
	close(socket_fd);
}


/* ************************************************************************** */
/* ************************************************************************** */


bool	Connection::isDone() const
{
	return (m_done);
}

void Connection::handleReadEvent(EventManager& event_manager)
{
	LOG_MSG("[handleReadEvent] ", std::string("Connection fd: ") + std::to_string(socket_fd), LIGHTMAGENTA, DEFAULT);

	char		buffer[REQUEST_BUFFER_SIZE] = {0};
	ssize_t		byetesRead = recv(socket_fd, &buffer, sizeof(buffer), 0);

	if (byetesRead > 0) {
		LOG_INFO_LM("RECIEVED BYTES: ", std::to_string(byetesRead));
		request.append(buffer, byetesRead);
		RequestParser::parseNext(request);
		// TODO:	if (request.checkifComplete())
		//				request.setComplet();
	} else if (byetesRead < 0) {
		LOG_ERROR("recieved bytes: -1 ---> socket error");
		request.setError(WSSC_INTERNAL_SERVER_ERROR);
	} else if (byetesRead == 0) {
		LOG_SUCCESS("Done reading from socket fd: " + std::to_string(socket_fd));
		request.setComplete();
	}

	if (request.complete() || request.error())
	{
		LOG_SUCCESS("recieved the full request || an error occured…");
		HTTP::handle(request, response);
		event_manager.setFdEvents(socket_fd, POLLOUT | POLLERR | POLLHUP);
	}
}


/************************************************************************ */
/************************************************************************ */


void Connection::handleWriteEvent(EventManager& event_manager)
{
	LOG_MSG("[handleWriteEvent] ", std::string("Connection fd: ") + std::to_string(socket_fd), LIGHTMAGENTA, DEFAULT);
	(void)event_manager;

	std::string	chunk = response.getNextChunk();

	LOG_INFO_LM("SENDING BYTES: ", std::to_string(chunk.length()));
	// LOG_MSG("RESPONSE:\n", chunk, LIGHTRED, LIGHTCYAN);

	LOG_INFO_LM("RESPONSE STATE", std::string("------> ") + response.getResponseStateString());
	send(socket_fd, chunk.c_str(), chunk.length(), 0);

	if (response.complete() || response.error())
	{
		if (response.error())
			LOG_ERROR("ERROR SENDING RESPONSE *************************************************");
		else
			LOG_SUCCESS("DONE SENDING RESPONSE *************************************************");
		m_done = true;
	}

}

void Connection::handleErrorEvent(EventManager& event_manager)
{
	LOG_MSG("[handleErrorEvent] ", std::string("Connection fd: ") + std::to_string(socket_fd), LIGHTMAGENTA, DEFAULT);
	(void)event_manager;
}

void Connection::handleDisConnectEvent(EventManager& event_manager)
{
	LOG_MSG("[handleDisConnectEvent] ", std::string("Connection fd: ") + std::to_string(socket_fd), LIGHTMAGENTA, DEFAULT);
	event_manager.unregisterFd(socket_fd);
}
