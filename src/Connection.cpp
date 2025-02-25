/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 19:11:37 by nmihaile          #+#    #+#             */
/*   Updated: 2025/02/25 15:38:10 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"

Connection::Connection(const int _cli_socket)
	:	m_done(false)
{
	socket_fd = _cli_socket;
	request.clear();
}

Connection::~Connection()
{
	Log::warning("CLOSING", std::string("connection socket fd: ") + std::to_string(socket_fd));
	close(socket_fd);
}


/* ************************************************************************** */
/* ************************************************************************** */


void	Connection::recieve(void)
{
	char		request_buf[4096];
	std::string	request_line;
	std::string	request_header_fields;
	std::string	message_body;

	recv(socket_fd, &request_buf, sizeof(request_buf), 0);

	std::string			line;
	std::stringstream	ss(request_buf);

	std::getline(ss, request_line);
	
	while (std::getline(ss, line) && !line.empty())
		request_header_fields += line + "\n";

	while (std::getline(ss, line) && !line.empty())
		message_body += line + "\n";

	// request = request_line;
	request = request_header_fields;
	// if (request_header_fields.empty())
	// 	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
	// request = message_body;
}

void	Connection::create_response(void)
{
	std::string			line;
	std::stringstream	ss(request);

	std::string			request_line;
	std::string			request_header_fields;
	std::string			message_body;

	std::getline(ss, request_line);
	
	while (std::getline(ss, line) && !line.empty())
		request_header_fields += line + "\n";

	while (std::getline(ss, line) && !line.empty())
		message_body += line + "\n";

	response = std::string("HTTP/1.0 200 OK\r\n\r\n<MESSAGE>\n") + request_line + "</MESSAGE>\0";
}

void	Connection::respond(void)
{
	send(socket_fd, response.c_str(), response.length(), 0);
}

bool	Connection::isDone() const
{
	return (m_done);
}

void Connection::handleReadEvent(EventManager& event_manager)
{
	Log::msg("[handleReadEvent] ", std::string("Connection fd: ") + std::to_string(socket_fd), LIGHTMAGENTA, DEFAULT);

	char		request_buf[1024] = {0};
	size_t		byetes_read = recv(socket_fd, &request_buf, sizeof(request_buf), 0);
	if (byetes_read > 0)
		request.append(request_buf, byetes_read);
	
	Log::debug("READING: ", byetes_read);

	// I have to split up the request in its propper stuff, and user athe proper stuff to create a goof response msg

	if (request.find("\r\n\r\n") != std::string::npos)
	{
		Log::debug("DONE READING");
		create_response();
		event_manager.setFdEvents(socket_fd, POLLOUT | POLLERR | POLLHUP);
	}
}

void Connection::handleWriteEvent(EventManager& event_manager)
{
	Log::msg("[handleWriteEvent] ", std::string("Connection fd: ") + std::to_string(socket_fd), LIGHTMAGENTA, DEFAULT);
	(void)event_manager;

	send(socket_fd, response.c_str(), response.length(), 0);
	m_done = true;

	// Log::msg("REQUEST\n", request, LIGHTCYAN, LIGHTCYAN);
	Log::raw(request);
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
