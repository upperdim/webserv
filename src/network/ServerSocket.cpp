#include <iostream>
#include <netdb.h>		// getaddrinfo()
#include <arpa/inet.h>	// inet_ntop()
#include <fcntl.h>		// O_NONBLOCK
#include "ServerSocket.hpp"
#include "Log.hpp"

ServerSocket::ServerSocket(ServerBlock defaultServerBlock) : sockaddr()
{
	setupSocket(defaultServerBlock);
	serverBlocks.push_back(defaultServerBlock);
}

void ServerSocket::setupSocket(ServerBlock defaultServerBlock)
{
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		throw (std::runtime_error("Error creating socket for Server constructor"));
	}

	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("failed to set socket to non-blocking mode with fcntl");

	int yes = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
		throw std::runtime_error("Failed to set socket options for server");

	// Set host
	addrinfo hints{};
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	addrinfo* res = nullptr;
	int err = getaddrinfo(defaultServerBlock.listenHostStr.c_str(), nullptr, &hints, &res);
	if (err != 0) {
		std::string errMsg = std::string("Error setting Server address with getaddrinfo: ") + gai_strerror(err);
		throw (std::runtime_error(errMsg));
	}
	
	sockaddr_in* ipv4 = reinterpret_cast<sockaddr_in*>(res->ai_addr);
	sockaddr = *ipv4;
	sockaddr.sin_family = AF_INET;
	freeaddrinfo(res);

	// Set port
	sockaddr.sin_port = htons(defaultServerBlock.listenPort);

	// Print host and port
	char ipStr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(sockaddr.sin_addr), ipStr, sizeof(ipStr));

	std::string hostAndPort = std::string(ipStr) + ":" + std::to_string(ntohs(sockaddr.sin_port));

	err = bind(fd, (struct sockaddr *) &sockaddr, sizeof sockaddr);
	if (err != 0) {
		throw std::runtime_error("bind() to " + hostAndPort + "failed.");
	}

	err = listen(fd, LISTEN_BACKLOG);
	if (err != 0) {
		throw std::runtime_error("listen() to " + hostAndPort + "failed.");
	}

	LOGT(Log::INFO, "ServerSocket created with fd = " << fd << ". Listening at " << hostAndPort);
}

int          ServerSocket::getFd()             { return fd; }
