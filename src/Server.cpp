/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 11:42:38 by nmihaile          #+#    #+#             */
/*   Updated: 2025/02/22 19:48:49 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server()
{
	// create srv socket
	srv_socket = socket(AF_INET, SOCK_STREAM, 0);

	// define server address
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(80);
	srv_addr.sin_addr.s_addr = INADDR_ANY;

	// bind srv socket to specific IP and PORT
	bind(srv_socket, (struct sockaddr*) &srv_addr, sizeof(srv_addr));

	// start listening
	listen(srv_socket, 5);
}

Server::~Server()
{
	close(srv_socket);
}


/* ************************************************************************** */
/* ************************************************************************** */


void	Server::srv_listen(void)
{
	// accept
	int	cli_socket;
	cli_socket = accept(srv_socket, NULL, NULL);
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
