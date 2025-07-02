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
	ServerSocket&	connectedServerSocket;
	bool			connectionError;
	Request			request;
	Response		response;

public:
	ClientConnection(int fd, ServerSocket& connectedServerSocket);

	void			receiveRequest();
	void			sendResponse();
	
	int				getFd();
	bool			getConnectionError();
	ServerSocket&	getConnectedServerSocket();
	Request&		getRequest();
	Response&		getResponse();
};

#endif
