/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 11:42:38 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/14 12:41:01 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server()
{
	// create srv socket
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0)
		throw ( std::runtime_error("failed to create spcket for server") );

	// define server address
	m_srv_addr.sin_family = AF_INET;
	m_srv_addr.sin_port = htons(80);
	m_srv_addr.sin_addr.s_addr = INADDR_ANY;

	// bind srv socket to specific IP and PORT
	bind(socket_fd, (struct sockaddr*) &m_srv_addr, sizeof(m_srv_addr));

	// start listening
	listen(socket_fd, 5);	// TODO: is 5 a good value here?

	LOG_SUCCESS("Server starts listening fd: " + std::to_string(socket_fd));
}

Server::~Server()
{
	LOG_WARNING_LM("CLOSING", std::string("server socket fd: ") + std::to_string(socket_fd));
	close(socket_fd);
}


/* ************************************************************************** */
/* ************************************************************************** */


void	Server::accept_connection(EventManager& event_manager)
{
	// accept
	int	cli_socket;
	cli_socket = accept(socket_fd, NULL, NULL);
	if (cli_socket == -1)
		throw ;
	
	LOG_SUCCESS("Connection established fd: " + std::to_string(cli_socket));

	Connection* connection = new Connection(cli_socket);

	event_manager.registerFd(connection, POLLIN | POLLERR | POLLHUP);

	// connection.recieve();
	// connection.create_response();
	// connection.respond();
}

void Server::handleReadEvent(EventManager& event_manager)
{
	LOG_MSG("[handleReadEvent] ", std::string("Server fd: ") + std::to_string(socket_fd), LIGHTMAGENTA, DEFAULT);
	accept_connection(event_manager);
}

void Server::handleWriteEvent(EventManager& event_manager)
{
	LOG_MSG("[handleWriteEvent] ", std::string("Server fd: ") + std::to_string(socket_fd), LIGHTMAGENTA, DEFAULT);
	(void)event_manager;
}

void Server::handleErrorEvent(EventManager& event_manager)
{
	LOG_MSG("[handleErrorEvent] ", std::string("Server fd: ") + std::to_string(socket_fd), LIGHTMAGENTA, DEFAULT);
	(void)event_manager;
}

void Server::handleDisConnectEvent(EventManager& event_manager)
{
	LOG_MSG("[handleDisConnectEvent] ", std::string("Server fd: ") + std::to_string(socket_fd), LIGHTMAGENTA, DEFAULT);
	(void)event_manager;
}
