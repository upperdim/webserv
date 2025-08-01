#ifndef CLIENTCONNECTION_HPP
#define CLIENTCONNECTION_HPP

#include "ServerSocket.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <string>

#define MAX_NUM_OF_TRIES_FOR_ZERO_BYTES_READ 5
#define SENT_CHUNK_LOG_TRESHOLD_LEN          2000
#define RECV_BUFFER_SIZE                     (1 * 1024 * 1024)

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
	std::string		dataToSend;

public:
	ClientConnection(int fd, ServerSocket& connectedServerSocket);
	~ClientConnection();

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
