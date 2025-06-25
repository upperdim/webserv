/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 19:11:37 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/25 11:07:45 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"
#include "HTTP.hpp"
#include "RequestParser.hpp"

Connection::Connection(const int _cli_socket, const ServerBlock& _serverBlock)
	:	request(_serverBlock),
		m_done(false),
		m_serverBlock(_serverBlock)
{
	socket_fd = _cli_socket;
}

Connection::~Connection()
{
	LOG_WARNING_LM("CLOSING", std::string("Connection fd: ") + std::to_string(socket_fd));
	close(socket_fd);
}


/* ************************************************************************** */
/* ************************************************************************** */


bool	Connection::isDone() const
{
	return (m_done);
}

void Connection::handleReadEvent(EventManager& event_manager)
{
	LOG_MSG("[handleReadEvent] ", std::string("Connection fd: ") + std::to_string(socket_fd), LIGHTMAGENTA, DEFAULT);

	char		requestBuf[REQUEST_BUFFER_SIZE] = {0};
	ssize_t		byetesRead = recv(socket_fd, &requestBuf, sizeof(requestBuf), 0);

	if (byetesRead > 0) {
		LOG_INFO_LM("RECIEVED BYTES: ", std::to_string(byetesRead));
		request.append(requestBuf, byetesRead);
		RequestParser::parseNext(request);
		// TODO:	if (request.checkifComplete())
		//				request.setComplet();
	} else if (byetesRead < 0) {
		LOG_ERROR("recieved bytes: -1 ---> socket error");
		request.setError(WSSC_INTERNAL_SERVER_ERROR);
	} else if (byetesRead == 0) {
		LOG_SUCCESS("Done reading from socket fd: " + std::to_string(socket_fd));
		request.setComplete();
	}

	if (request.complete() || request.error())
	{
		LOG_SUCCESS("DONE READING **********************************************************");
		
		if (request.error()) {
			response = handleErrorRequest(request);
		} else {
			if (request.getRequestTarget().empty()) {
				response.setStatus(WSSC_NOT_FOUND);
			} else if (!request.isAllowedMethod()) {
				response.setStatus(WSSC_METHOD_NOT_ALLOWED);
			} else {
				switch (request.method) {
					case HTTP::Method::GET:
						response = handleGetRequest(request);
						break;
					case HTTP::Method::POST:
						response = handlePostRequest(request);
						break;
					case HTTP::Method::DELETE:
						response = handleDeleteRequest(request);
						break;
				}
			}
		}
		event_manager.setFdEvents(socket_fd, POLLOUT | POLLERR | POLLHUP);
	}
}

/************************************************************************ */

Response	Connection::handleGetRequest(const Request& request)
{
	LOG_MSG("[handle Get Request] ", "...", LIGHTMAGENTA, DEFAULT);

	Response response;

	// TODO: repetitive?
	if (request.getStatusCode() >= WSSC_BAD_REQUEST)
	{
		createErrorResponse(response, request.getStatusCode());
		return (response);
	}

	// TODO: restructure:
	// sanitize path
	std::string path = Utils::sanitizePath(request, m_serverBlock);
	LOG_DEBUG("PATH ----> " + path);

	// does the path resource exist
	if (Utils::resourceExist(path))
	{
		// fetch content
		response.addHeader("Content-Type", HTTP::getMimeType(path));
		response.setBodyFileBufferReader(path);
	}
	else
		createErrorResponse(response, WSSC_I_M_A_TEAPOT);

	return (response);
}

std::string	Connection::fetchErrorPage(int _status_code) const
{
	return (HTTP::getErrorPageTemplate(_status_code));
}

void	Connection::createErrorResponse(Response& response, int _status_code)
{
	response.setProtokoll("HTTP/1.1");
	response.setStatus(_status_code);
	response.addHeader("Content-Type", HTTP::getMimeType(".html"));
	response.setBodyString(fetchErrorPage(_status_code));
}

Response	Connection::handleErrorRequest(const Request& request)
{
	LOG_MSG("[handle Error Request] ", "...", LIGHTMAGENTA, DEFAULT);

	Response	response;
	int			status_code = request.getStatusCode();

	if (status_code < WSSC_BAD_REQUEST)
		status_code = WSSC_INTERNAL_SERVER_ERROR;

	createErrorResponse(response, status_code);
	return (response);
}

Response	Connection::handlePostRequest(const Request& request)
{
	LOG_MSG("[handle Post Request] ", "...", LIGHTMAGENTA, DEFAULT);
	(void) request;
	throw std::runtime_error("Work in progress...");
}

Response	Connection::handleDeleteRequest(const Request& request)
{
	LOG_MSG("[handle DELETE Request] ", "...", LIGHTMAGENTA, DEFAULT);
	Response response;

	std::filesystem::path resourcePath(Utils::sanitizePath(request, m_serverBlock));
	if (!(std::filesystem::exists(resourcePath) && std::filesystem::is_regular_file(resourcePath))) {
		// resourcePath NOT FOUND
		response.setStatus(WSSC_NOT_FOUND);
		return response;
	}

	LOG_WARNING_LM("DELETING", resourcePath.c_str());
	if (!std::filesystem::remove(resourcePath)) {
		// failed to remove
		response.setStatus(WSSC_INTERNAL_SERVER_ERROR);
		return response;
	}
	LOG_SUCCESS(std::string("deleted: ") + resourcePath.c_str());

	response.setStatus(WSSC_OK);
	return response;
}

/************************************************************************ */

void Connection::handleWriteEvent(EventManager& event_manager)
{
	LOG_MSG("[handleWriteEvent] ", std::string("Connection fd: ") + std::to_string(socket_fd), LIGHTMAGENTA, DEFAULT);
	(void)event_manager;

	std::string	chunk = response.getNextChunk();

	LOG_INFO_LM("SENDING BYTES: ", std::to_string(chunk.length()));
	// LOG_MSG("RESPONSE:\n", chunk, LIGHTRED, LIGHTCYAN);

	LOG_INFO_LM("RESPONSE STATE", std::string("------> ") + response.getResponseStateString());
	send(socket_fd, chunk.c_str(), chunk.length(), 0);

	if (response.complete() || response.error())
	{
		if (response.error())
			LOG_ERROR("ERROR SENDING RESPONSE *************************************************");
		else
			LOG_SUCCESS("DONE SENDING RESPONSE *************************************************");
		m_done = true;
	}

	// LOG_MSG("REQUEST\n", request.getRequest(), LIGHTCYAN, LIGHTCYAN);
	// LOG_RAW(chunk.c_str(), 16);
}

void Connection::handleErrorEvent(EventManager& event_manager)
{
	LOG_MSG("[handleErrorEvent] ", std::string("Connection fd: ") + std::to_string(socket_fd), LIGHTMAGENTA, DEFAULT);
	(void)event_manager;
}

void Connection::handleDisConnectEvent(EventManager& event_manager)
{
	LOG_MSG("[handleDisConnectEvent] ", std::string("Connection fd: ") + std::to_string(socket_fd), LIGHTMAGENTA, DEFAULT);
	event_manager.unregisterFd(socket_fd);
}
