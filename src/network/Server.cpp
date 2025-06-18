/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 11:42:38 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/18 16:43:15 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(ServerBlock serverBlock) 
	:	m_serverBlock(serverBlock)
{
	// create srv socket
	socket_fd = socket(AF_INET, SOCK_STREAM, 0); // TODO: add SOCK_NONBLOCK
	if (socket_fd < 0)
		throw ( std::runtime_error("failed to create socket for server") );

	// define server address
	m_srvAddr.sin_family = AF_INET;
	m_srvAddr.sin_port = htons(serverBlock.listenPort);
	m_srvAddr.sin_addr.s_addr = serverBlock.listenHost;

	// bind srv socket to specific IP and PORT
	bind(socket_fd, (struct sockaddr*) &m_srvAddr, sizeof(m_srvAddr));

	// start listening
	listen(socket_fd, 5);	// TODO: is 5 a good value here?

	LOG_SUCCESS("Server starts listening fd: " + std::to_string(socket_fd));
}

Server::~Server()
{
	if (socket_fd > 0)
	{
		LOG_WARNING_LM("CLOSING", std::string("server socket fd: ") + std::to_string(socket_fd));
		close(socket_fd);
	}
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
	
	// TODO: set cli_socket to O_NONBLOCK

	LOG_SUCCESS("Connection established fd: " + std::to_string(cli_socket));

	Connection* connection = new Connection(cli_socket, m_serverBlock);

	event_manager.registerFd(connection, POLLIN | POLLERR | POLLHUP);
}

void	Server::handleReadEvent(EventManager& event_manager)
{
	LOG_MSG("[handleReadEvent] ", std::string("Server fd: ") + std::to_string(socket_fd), LIGHTMAGENTA, DEFAULT);
	accept_connection(event_manager);
}

void	Server::handleWriteEvent(EventManager& event_manager)
{
	LOG_MSG("[handleWriteEvent] ", std::string("Server fd: ") + std::to_string(socket_fd), LIGHTMAGENTA, DEFAULT);
	(void)event_manager;
}

void	Server::handleErrorEvent(EventManager& event_manager)
{
	LOG_MSG("[handleErrorEvent] ", std::string("Server fd: ") + std::to_string(socket_fd), LIGHTMAGENTA, DEFAULT);
	(void)event_manager;
}

void	Server::handleDisConnectEvent(EventManager& event_manager)
{
	LOG_MSG("[handleDisConnectEvent] ", std::string("Server fd: ") + std::to_string(socket_fd), LIGHTMAGENTA, DEFAULT);
	(void)event_manager;
}
