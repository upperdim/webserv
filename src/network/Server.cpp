/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 11:42:38 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/22 11:03:05 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(ServerBlock serverBlock) 
	:	m_serverBlock(serverBlock)
{
	//	create srv socket
	socket_fd = socket(AF_INET, SOCK_STREAM, 0); // TODO: add SOCK_NONBLOCK
	if (socket_fd < 0)
		throw std::runtime_error("failed to create socket for server");

	//	set socket to non-blocking mode
	int flags = fcntl(socket_fd, F_GETFL, 0);
	if (flags < 0) 
		throw std::runtime_error("failed to get socket flags with fcntl");
	if (fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK) < 0)
		throw std::runtime_error("failed to set socket to non-blocking mode with fcntl");

	//	set socket options
	int yes = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
		throw std::runtime_error("failed to set socket options");

	// resolve hostname to IP address
	addrinfo hints{};
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	addrinfo* res = nullptr;
	int err = getaddrinfo(serverBlock.listenHostStr.c_str(), nullptr, &hints, &res);
	if (err != 0) 
		throw std::runtime_error(std::string("Error setting Server address with getaddrinfo: ") + gai_strerror(err)) ;
	
	//	setup server address structure
	sockaddr_in* ipv4    = reinterpret_cast<sockaddr_in*>(res->ai_addr);
	m_srvAddr.sin_family = AF_INET;
	m_srvAddr.sin_port   = htons(serverBlock.listenPort);
	m_srvAddr.sin_addr   = ipv4->sin_addr;

	freeaddrinfo(res);

	std::string hostAndPort = serverBlock.listenHostStr + ":" + std::to_string(serverBlock.listenPort);

	// bind srv socket to ADDRESS and PORT
	err = bind(socket_fd, (struct sockaddr*) &m_srvAddr, sizeof(m_srvAddr));
	if (err != 0)
		throw std::runtime_error("bind() to " + hostAndPort + "failed.");

	// start listening for connections
	static const int LISTEN_BACKLOG = 10;		// TODO: change this value
	err = listen(socket_fd, LISTEN_BACKLOG);	// TODO: is LISTEN_BACKLOG a good value here? Mobe it as a DEFINE?
	if (err != 0)
		throw std::runtime_error("listen() to " + hostAndPort + "failed.");

	LOG_SUCCESS(std::string("Server fd = ") + LIGHTGREEN + BOLD + std::to_string(socket_fd) + DEFAULT + GREEN + " starts listening on: " + LIGHTGREEN + BOLD + hostAndPort);
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
		throw std::runtime_error("Error accepting connection");
	
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
