/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 19:11:37 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/08 14:26:19 by nmihaile         ###   ########.fr       */
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
	Log::warning("CLOSING", std::string("Connection fd: ") + std::to_string(socket_fd));
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
	Log::msg("[handleReadEvent] ", std::string("Connection fd: ") + std::to_string(socket_fd), LIGHTMAGENTA, DEFAULT);

	char		request_buf[REQUEST_BUFFER_SIZE] = {0};
	ssize_t		byetes_read = recv(socket_fd, &request_buf, sizeof(request_buf), 0);
	Log::info("RECIEVED BYTES: " + std::to_string(byetes_read));

	if (byetes_read == -1)
		request.setError(Request::State::ERROR, WSSC_INTERNAL_SERVER_ERROR);
	else if (byetes_read == 0)
		request.setComplete();
	else if (byetes_read > 0)
		request.append(request_buf, byetes_read);

	if (request.complete() || request.error())
	{
		Log::success("DONE READING **********************************************************");
		
		// response.create(request);
		Router router;
		IHandler* handler = router.route(request);
		response = handler->handle(request);
		delete (handler);

		event_manager.setFdEvents(socket_fd, POLLOUT | POLLERR | POLLHUP);
	}
}

void Connection::handleWriteEvent(EventManager& event_manager)
{
	Log::msg("[handleWriteEvent] ", std::string("Connection fd: ") + std::to_string(socket_fd), LIGHTMAGENTA, DEFAULT);
	(void)event_manager;

	Log::info("SENDING BYTES: " + std::to_string(response.data.length()));

	send(socket_fd, response.data.c_str(), response.data.length(), 0);
	m_done = true;

	Log::success("DONE SENDING **********************************************************");

	// Log::msg("REQUEST\n", request.getRequest(), LIGHTCYAN, LIGHTCYAN);
	// Log::raw(request, 16);
}

void Connection::handleErrorEvent(EventManager& event_manager)
{
	Log::msg("[handleErrorEvent] ", std::string("Connection fd: ") + std::to_string(socket_fd), LIGHTMAGENTA, DEFAULT);
	(void)event_manager;
}

void Connection::handleDisConnectEvent(EventManager& event_manager)
{
	Log::msg("[handleDisConnectEvent] ", std::string("Connection fd: ") + std::to_string(socket_fd), LIGHTMAGENTA, DEFAULT);
	event_manager.unregisterFd(socket_fd);
}
