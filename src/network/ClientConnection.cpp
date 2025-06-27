#include "ClientConnection.hpp"
#include "Logger.hpp"
#include "Utils.hpp"

ClientConnection::ClientConnection(int fd, Server& connectedServer) 
	:	fd(fd),
		connectedServer(connectedServer),
		connectionError(false) {}

void ClientConnection::receiveRequest() {
	log("Reading from ClientConnection fd = " << fd);
	char buffer[REQUEST_BUFFER_SIZE];
	ssize_t bytesRead = recv(fd, &buffer, REQUEST_BUFFER_SIZE - 1, 0);
	log(bytesRead << " bytes read");

	if (bytesRead > 0) {
		request.recvBuffer.append(buffer, bytesRead);
		log("recvBuffer.length() = " << request.recvBuffer.length());
		buffer[bytesRead] = '\0';
		log("buffer = <<<" << buffer << ">>>");
		request.parseNext();
	} else if (bytesRead < 0) {
		connectionError = true;
		logl(ERROR, "ClientConnection error, fd = " << fd);
	} else if (bytesRead == 0) {
		request.isComplete = true;
		log("ClientConnection fd = " << fd << " has received all of the request");
	}
}

void ClientConnection::sendResponse() {
	log("Writing to ClientConnection fd = " << fd);
	// TODO: (Feature) Write in chunks
	log("response data = " << response.data);
	response.isComplete = true;
}

int         ClientConnection::getFd()                 { return fd; }
bool        ClientConnection::getConnectionError()    { return connectionError; }
Server&     ClientConnection::getConnectedServer()    { return connectedServer; }
Request		ClientConnection::getRequest()            { return request; }
Response	ClientConnection::getResponse()           { return response; }
void		ClientConnection::setResponse(Response r) { response = r; }
