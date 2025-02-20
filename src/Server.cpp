/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 11:42:38 by nmihaile          #+#    #+#             */
/*   Updated: 2025/02/20 17:42:58 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"

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

	// recive
	char		buf[2048];
	recv(cli_socket, &buf, sizeof(buf), 0);
	std::string msg(buf);

	srv_respond(cli_socket, msg);
	close(cli_socket);
}

void	Server::srv_respond(int cli_socket, std::string msg)
{
	// respond
	std::string response = std::string("HTTP/1.0 200 OK\r\n\r\n__GELL__") + msg;
	send(cli_socket, response.c_str(), response.length(), 0);
}

void	Server::srv_close(int cli_socket)
{
	close(cli_socket);
}
