#ifndef CLIENTCONNECTION_HPP
#define CLIENTCONNECTION_HPP

#include <string>
#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"

#define REQUEST_BUFFER_SIZE	2048

class ClientConnection {
private:
	int			fd;
	Server&		connectedServer;
	bool		connectionError;
	Request		request;
	Response	response;

public:
	ClientConnection(int fd, Server& connectedServer);

	void		receiveRequest();
	void		sendResponse();
	
	int			getFd();
	bool		getConnectionError();
	Server&		getConnectedServer();
	std::string getRecvBuffer();
	Request		getRequest();
	Response	getResponse();
	void		setResponse(Response r);
};

#endif
