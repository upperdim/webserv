#include "ClientConnection.hpp"
#include "RequestParser.hpp"
#include "CGIHandler.hpp"
#include "colors.hpp"
#include "Log.hpp"

ClientConnection::ClientConnection(int fd, ServerSocket& _connectedServerSocket) 
	:	fd(fd),
		connectedServerSocket(_connectedServerSocket),
		connectionError(false),
		disconnected(false),
		request(connectedServerSocket.serverBlocks),
		zeroBytesReadCounter(0)
{
}

void ClientConnection::receiveRequest()
{
	LOG("Reading from ClientConnection fd = " << fd);
	char buffer[RECV_BUFFER_SIZE] = {0};
	ssize_t bytesRead = recv(fd, &buffer, RECV_BUFFER_SIZE - 1, 0);
	LOG(bytesRead << " bytes read");

	if (bytesRead > 0) {
		buffer[bytesRead] = '\0';
		LOG("buffer = " << LIGHTMAGENTA <<  "<<<\n" << LIGHTCYAN << buffer << LIGHTMAGENTA << ">>>" << DEFAULT);
		
		request.rawRequest.append(buffer, bytesRead);
		LOG("rawRequest.length() = " << request.rawRequest.length());

		RequestParser::parseNext(request);
	} else if (bytesRead < 0) {
		connectionError = true;
		LOGT(Log::ERROR, "ClientConnection error, fd = " << fd);
	} else if (bytesRead == 0) {
		if (zeroBytesReadCounter < MAX_NUM_OF_TRIES_FOR_ZERO_BYTES_READ) {
			++zeroBytesReadCounter; // Try again on the next poll turn
		} else {
			disconnected = true;
			LOGT(Log::INFO, "ClientConnection fd = " << fd << " has sent 0 bytes for " << zeroBytesReadCounter << " time(s).");
			LOGT(Log::INFO, "ClientConnection fd = " << fd << " has disconnected.");
		}
	}
}

void ClientConnection::sendResponse()
{
	LOG("Writing to ClientConnection fd = " << fd);
	
	std::string	chunk = response.getNextChunk();
	ssize_t bytesSent = send(fd, chunk.c_str(), chunk.length(), 0);
	LOG(bytesSent << " bytes sent");

	// TODO: Make sure whole response chunk is sent at each try

	if (bytesSent > 0) {
		if (bytesSent < SENT_CHUNK_LOG_TRESHOLD_LEN) {
			LOG("chunk = " << LIGHTMAGENTA <<  "<<<\n" << LIGHTCYAN << chunk.c_str() << LIGHTMAGENTA << ">>>" << DEFAULT);
		} else {
			LOG("chunk too long to log");
		}
	} else if (bytesSent < 0) {
		connectionError = true;
		LOGT(Log::ERROR, "ClientConnection error, fd = " << fd);
	} else if (bytesSent == 0) {
		response.setComplete();
		LOG("ClientConnection fd = " << fd << " has done sending the response");
	}
}

bool	ClientConnection::isWaitingForCgi()
{
	if (!request.isCGIRequest()) {
		return false;
	}

	// This check is necessary for visits after the CGI was complete.
	//
	// If the CGI was already completed
	// and a response with multiple chunks was created,
	// it will take multiple poll iterations to send the response.
	// Therefore we will hit this function multiple times
	// until all the response chunks are sent to the client.
	//
	// We should prevent creating the response over and over again
	// for the repeated visits.
	if (request.cgiSession.state == Request::CgiState::COMPLETE
	    || request.cgiSession.state == Request::CgiState::FAILED) {
		return false;
	}
	
	if (CGIHandler::checkCgiCompletion(request, response)) {
		CGIHandler::createCgiResponse(request, response);
		return false;
	}

	return true;
}

int         	ClientConnection::getFd()                 		{ return fd; }
bool        	ClientConnection::getDisconnected()    		    { return disconnected; }
bool        	ClientConnection::getConnectionError()    		{ return connectionError; }
ServerSocket&	ClientConnection::getConnectedServerSocket()    { return connectedServerSocket; }
Request&    	ClientConnection::getRequest()            		{ return request; }
Response&   	ClientConnection::getResponse()           		{ return response; }
