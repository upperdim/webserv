/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tunsal <tunsal@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 02:25:16 by tunsal            #+#    #+#             */
/*   Updated: 2025/06/27 02:25:16 by tunsal           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <netdb.h>		// getaddrinfo()
#include <arpa/inet.h>	// inet_ntop()
#include "Server.hpp"
#include "Log.hpp"

Server::Server(ServerBlock sb) : sockaddr(), serverBlock(sb) {
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		throw (std::runtime_error("Error creating socket for Server constructor"));
	}

	int yes = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
		throw std::runtime_error("Failed to set socket options for server");

	// Set host
	addrinfo hints{};
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	addrinfo* res = nullptr;
	int err = getaddrinfo(serverBlock.listenHost.c_str(), nullptr, &hints, &res);
	if (err != 0) {
		std::string errMsg = std::string("Error setting Server address with getaddrinfo: ") + gai_strerror(err);
		throw (std::runtime_error(errMsg));
	}
	
	sockaddr_in* ipv4 = reinterpret_cast<sockaddr_in*>(res->ai_addr);
	sockaddr = *ipv4;
	freeaddrinfo(res);

	// Set port
	sockaddr.sin_port = htons(serverBlock.listenPort);

	// Print host and port
	char ipStr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(sockaddr.sin_addr), ipStr, sizeof(ipStr));
	// std::cout << "Resolved to: " << ipStr << ":" << ntohs(sockaddr.sin_port) << std::endl;

	err = bind(fd, (struct sockaddr *) &sockaddr, sizeof sockaddr);
	if (err != 0) {
		throw (std::runtime_error("Error binding socket for Server constructor: "));
	}

	err = listen(fd, LISTEN_BACKLOG);
	if (err != 0) {
		throw (std::runtime_error("Error binding socket for Server constructor"));
	}

	logl(INFO, "Server created with fd = " << fd << ". Listening at " << ipStr << ":" << ntohs(sockaddr.sin_port));
}

int Server::getFd() {
	return fd;
}
