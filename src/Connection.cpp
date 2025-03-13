/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 19:11:37 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/13 18:41:02 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"

Connection::Connection(const int _cli_socket)
	:	m_done(false)
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
		
		// response.create(request);
		Router router;
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

	std::string	final_response = response.to_string();

	LOG_INFO_LM("SENDING BYTES: ", std::to_string(final_response.length()));
	// LOG_MSG("RESPONSE:\n", final_response, LIGHTRED, LIGHTCYAN);

	send(socket_fd, final_response.c_str(), final_response.length(), 0);
	m_done = true;

	LOG_SUCCESS("DONE SENDING **********************************************************");

	// LOG_MSG("REQUEST\n", request.getRequest(), LIGHTCYAN, LIGHTCYAN);
	// LOG_RAW(final_response.c_str(), 16);
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
