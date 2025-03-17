/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 19:11:37 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/17 19:29:09 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"

Connection::Connection(const int _cli_socket, const Server& _server)
	:	m_done(false),
		m_server(_server)
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

	char		request_buf[REQUEST_BUFFER_SIZE] = {0};
	ssize_t		byetes_read = recv(socket_fd, &request_buf, sizeof(request_buf), 0);

	LOG_INFO_LM("RECIEVED BYTES: ", std::to_string(byetes_read));
	if (byetes_read == -1)
		request.setError(WSSC_INTERNAL_SERVER_ERROR);
	else if (byetes_read == 0)
		request.setComplete();
	else if (byetes_read > 0)
		request.append(request_buf, byetes_read);

	if (request.complete() || request.error())
	{
		LOG_SUCCESS("DONE READING **********************************************************");
		
		Router router(m_server);			// TODO: should the Router class be a Utility-Class like Log ???
		AHandler* handler = router.route(request);
		response = handler->handle(request);
		delete (handler);

		event_manager.setFdEvents(socket_fd, POLLOUT | POLLERR | POLLHUP);
	}
}

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

	// LOG_MSG("REQUEST\n", request.getRequest(), LIGHTCYAN, LIGHTCYAN);
	// LOG_RAW(chunk.c_str(), 16);
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
