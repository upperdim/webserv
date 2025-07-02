#ifndef SERVER_HPP
#define SERVER_HPP

#include <unordered_map>
#include <netinet/in.h>
#include "Config.hpp"

#define LISTEN_BACKLOG 10 // Maximum number of connection requests in queue

class Server
{
private:
	int												fd; 		// File descriptor
	struct sockaddr_in								sockaddr;
	std::unordered_map<std::string, ServerBlock>	serverNametoServerBlock;

public:
	Server(ServerBlock serverBlock);
	
	int					getFd();
	ServerBlock&		getServerBlockRef();
};

#endif
