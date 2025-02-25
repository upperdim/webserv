/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 11:42:38 by nmihaile          #+#    #+#             */
/*   Updated: 2025/02/25 14:41:47 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server()
{
	// create srv socket
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0)
		throw ( std::runtime_error("failed to create spcket for server") );
	// Log::msg("[Server socket fd] ", std::to_string(socket_fd), LIGHTMAGENTA, DEFAULT);

	// define server address
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(80);
	srv_addr.sin_addr.s_addr = INADDR_ANY;

	// bind srv socket to specific IP and PORT
	bind(socket_fd, (struct sockaddr*) &srv_addr, sizeof(srv_addr));

	// start listening
	listen(socket_fd, 5);	// TODO: is 5 a good value here?

	Log::success("Server starts listening fd: " + std::to_string(socket_fd));
}

Server::~Server()
{
	Log::warning("CLOSING", std::string("server socket fd: ") + std::to_string(socket_fd));
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
	
	Log::success("Connection established fd: " + std::to_string(cli_socket));

	Connection* connection = new Connection(cli_socket);

	event_manager.registerFd(connection, POLLIN | POLLERR | POLLHUP);

	// connection.recieve();
	// connection.create_response();
	// connection.respond();
}

void Server::handleReadEvent(EventManager& event_manager)
{
	Log::msg("[handleReadEvent] ", std::string("Server fd: ") + std::to_string(socket_fd), LIGHTMAGENTA, DEFAULT);
	accept_connection(event_manager);
}

void Server::handleWriteEvent(EventManager& event_manager)
{
	Log::msg("[handleWriteEvent] ", std::string("Server fd: ") + std::to_string(socket_fd), LIGHTMAGENTA, DEFAULT);
	(void)event_manager;
}

void Server::handleErrorEvent(EventManager& event_manager)
{
	Log::msg("[handleErrorEvent] ", std::string("Server fd: ") + std::to_string(socket_fd), LIGHTMAGENTA, DEFAULT);
	(void)event_manager;
}

void Server::handleDisConnectEvent(EventManager& event_manager)
{
	Log::msg("[handleDisConnectEvent] ", std::string("Server fd: ") + std::to_string(socket_fd), LIGHTMAGENTA, DEFAULT);
	(void)event_manager;
}
