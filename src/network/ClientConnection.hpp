#ifndef CLIENTCONNECTION_HPP
#define CLIENTCONNECTION_HPP

#include <string>
#include "ServerSocket.hpp"
#include "Request.hpp"
#include "Response.hpp"

#define RECV_BUFFER_SIZE	2048

class ClientConnection
{
private:
int					fd;
	ServerSocket&	connectedServer;
	bool			connectionError;
	Request			request;
	Response		response;

public:
	ClientConnection(int fd, ServerSocket& connectedServer);

	void			receiveRequest();
	void			sendResponse();
	
	int				getFd();
	bool			getConnectionError();
	ServerSocket&	getConnectedServer();
	Request&		getRequest();
	Response&		getResponse();
};

#endif
