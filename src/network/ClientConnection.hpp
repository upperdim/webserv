#ifndef CLIENTCONNECTION_HPP
#define CLIENTCONNECTION_HPP

#include "ServerSocket.hpp"
#include "Request.hpp"
#include "Response.hpp"

#define MAX_NUM_OF_TRIES_FOR_ZERO_BYTES_READ 5
#define RECV_BUFFER_SIZE                     2048

class ClientConnection
{
private:
int					fd;
	ServerSocket&	connectedServerSocket;
	bool			connectionError;
	bool			disconnected;
	Request			request;
	Response		response;
	int				zeroBytesReadCounter;

public:
	ClientConnection(int fd, ServerSocket& connectedServerSocket);

	void			receiveRequest();
	void			sendResponse();
	bool			isWaitingForCgi();
	
	int				getFd();
	bool			getDisconnected();
	bool			getConnectionError();
	ServerSocket&	getConnectedServerSocket();
	Request&		getRequest();
	Response&		getResponse();
};

#endif
