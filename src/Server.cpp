/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 11:42:38 by nmihaile          #+#    #+#             */
/*   Updated: 2025/02/22 20:39:43 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server()
{
	// create srv socket
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);

	// define server address
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(80);
	srv_addr.sin_addr.s_addr = INADDR_ANY;

	// bind srv socket to specific IP and PORT
	bind(socket_fd, (struct sockaddr*) &srv_addr, sizeof(srv_addr));

	// start listening
	listen(socket_fd, 5);	// TODO: is 5 a good value here?
}

Server::~Server()
{
	close(socket_fd);
}


/* ************************************************************************** */
/* ************************************************************************** */


void	Server::srv_listen(void)
{
	// accept
	int	cli_socket;
	cli_socket = accept(socket_fd, NULL, NULL);
	if (cli_socket == -1)
		return ;
	
	Connection connection(cli_socket);

	connection.recieve();
	connection.create_response();
	connection.respond();
}

void Server::handleReadEvent()
{
}

void Server::handleWriteEvent()
{
}

void Server::handleErrorEvent()
{
}
