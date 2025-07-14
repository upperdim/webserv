#include "ClientConnection.hpp"
#include "RequestParser.hpp"
#include "colors.hpp"
#include "Log.hpp"

ClientConnection::ClientConnection(int fd, ServerSocket& _connectedServerSocket) 
	:	fd(fd),
		connectedServerSocket(_connectedServerSocket),
		connectionError(false),
		request(connectedServerSocket.serverBlocks)
{
}

void ClientConnection::receiveRequest()
{
	LOG("Reading from ClientConnection fd = " << fd);

	char buffer[RECV_BUFFER_SIZE];
	ssize_t bytesRead = recv(fd, &buffer, RECV_BUFFER_SIZE - 1, 0);
	LOG(bytesRead << " bytes read");

	if (bytesRead > 0) {
		if (request.storeBodyInFile) {
			// append to temp file
		} else {
			request.rawRequest.append(buffer, bytesRead);
		}
		RequestParser::parseNext(request);
		
		buffer[bytesRead] = '\0';
		LOG("rawRequest.length() = " << request.rawRequest.length());
		LOG("buffer = " << LIGHTMAGENTA <<  "<<<\n" << LIGHTCYAN << buffer << LIGHTMAGENTA << ">>>" << DEFAULT);
	} else if (bytesRead < 0) {
		connectionError = true;
		LOGT(Log::ERROR, "ClientConnection error, fd = " << fd);
	} else if (bytesRead == 0) {
		request.doneReceiving = true;
		LOG("ClientConnection fd = " << fd << " has received all of the request");
	}
}

void ClientConnection::sendResponse()
{
	LOG("Writing to ClientConnection fd = " << fd);
	
	std::string	chunk = response.getNextChunk();
	send(fd, chunk.c_str(), chunk.length(), 0); // TODO: return val & checks
}

int         	ClientConnection::getFd()                 		{ return fd; }
bool        	ClientConnection::getConnectionError()    		{ return connectionError; }
ServerSocket&	ClientConnection::getConnectedServerSocket()    { return connectedServerSocket; }
Request&    	ClientConnection::getRequest()            		{ return request; }
Response&   	ClientConnection::getResponse()           		{ return response; }
