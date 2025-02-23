/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 19:11:37 by nmihaile          #+#    #+#             */
/*   Updated: 2025/02/23 12:11:58 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"

Connection::Connection(int _cli_socket)
	:	m_done(false)
{
	socket_fd = _cli_socket;
}

Connection::~Connection()
{
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
	response = std::string("HTTP/1.0 200 OK\r\n\r\n<MESSAGE>\n") + request + "</MESSAGE>\0";
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
	Log::msg("[handleReadEvent | Connection] ", std::to_string(socket_fd), LIGHTMAGENTA, DEFAULT);
	(void)event_manager;
}

void Connection::handleWriteEvent(EventManager& event_manager)
{
	Log::msg("[handleWriteEvent | Connection] ", std::to_string(socket_fd), LIGHTMAGENTA, DEFAULT);
	(void)event_manager;
}

void Connection::handleErrorEvent(EventManager& event_manager)
{
	Log::msg("[handleErrorEvent | Connection] ", std::to_string(socket_fd), LIGHTMAGENTA, DEFAULT);
	(void)event_manager;
}

void Connection::handleDisConnectEvent(EventManager& event_manager)
{
	Log::msg("[handleDisConnectEvent | Connection] ", std::to_string(socket_fd), LIGHTMAGENTA, DEFAULT);
	(void)event_manager;
}
