/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 19:11:37 by nmihaile          #+#    #+#             */
/*   Updated: 2025/02/21 13:22:18 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"

Connection::Connection(int _cli_socket)
	:	cli_socket(_cli_socket),
		m_done(false)
{
}

Connection::~Connection()
{
	close(cli_socket);
}


/* ************************************************************************** */
/* ************************************************************************** */


void	Connection::recieve(void)
{
	char		request_buf[4096];
	std::string	request_line;
	std::string	request_header_fields;
	std::string	message_body;

	recv(cli_socket, &request_buf, sizeof(request_buf), 0);

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
	send(cli_socket, response.c_str(), response.length(), 0);
}
