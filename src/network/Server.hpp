#ifndef SERVER_HPP
#define SERVER_HPP

#include <netinet/in.h>
#include "Config.hpp"

#define LISTEN_BACKLOG 10 // Maximum number of connection requests in queue

class Server {
private:
	int					fd; 		// File descriptor of the Server socket
	struct sockaddr_in	sockaddr;
	ServerBlock			serverBlock;

public:
	Server(ServerBlock serverBlock);
	
	int					getFd();
};

#endif
