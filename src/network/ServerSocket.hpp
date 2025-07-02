#ifndef SERVER_HPP
#define SERVER_HPP

#include <netinet/in.h>
#include <map>
#include "Config.hpp"

#define LISTEN_BACKLOG 10 // Maximum number of connection requests in queue

class ServerSocket
{
private:
	int									fd; // File descriptor of the server socket
	struct sockaddr_in					sockaddr;
	std::map<std::string, ServerBlock>	serverNameToServerBlock;

public:
	ServerSocket(ServerBlock serverBlock);
	
	int									getFd();
	ServerBlock&						getServerBlockRef();
};

#endif
