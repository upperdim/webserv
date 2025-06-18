/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tunsal <tunsal@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 19:11:37 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/18 22:25:34 by tunsal           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"

Connection::Connection(const int _cli_socket, const ServerBlock& _serverBlock)
	:	m_done(false),
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

	char		request_buf[REQUEST_BUFFER_SIZE] = {0};
	ssize_t		byetes_read = recv(socket_fd, &request_buf, sizeof(request_buf), 0);

	LOG_INFO_LM("RECIEVED BYTES: ", std::to_string(byetes_read));
	if (byetes_read == -1)
		request.setError(WSSC_INTERNAL_SERVER_ERROR);
	else if (byetes_read == 0)
		request.setComplete();
	else if (byetes_read > 0)
		request.append(request_buf, byetes_read);

	if (request.complete() || request.error())
	{
		LOG_SUCCESS("DONE READING **********************************************************");
		
		if (request.error()) {
			response = handleErrorRequest(request);
		} else {
			switch (request.getMethod()) {
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
		event_manager.setFdEvents(socket_fd, POLLOUT | POLLERR | POLLHUP);
	}
}

/************************************************************************ */

Response	Connection::handleGetRequest(const Request& request)
{
	Response response;

	if (request.error())
	{
		createErrorResponse(response, request.getStatusCode());
		return (response);
	}

	// TODO: restructure:
	// sanitize path
	std::string path = Utils::sanitizePath(request, response, m_serverBlock);
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
	response.setStatus(_status_code, HTTP::getStatusMessage(_status_code));
	response.addHeader("Content-Type", HTTP::getMimeType(".html"));
	response.setBodyString(fetchErrorPage(_status_code));
}

Response	Connection::handleErrorRequest(const Request& request)
{
	Response	response;
	int			status_code = request.getStatusCode();

	if (status_code < WSSC_BAD_REQUEST)
		status_code = WSSC_INTERNAL_SERVER_ERROR;

	createErrorResponse(response, status_code);
	return (response);
}

Response	Connection::handlePostRequest(const Request& request)
{
	(void) request;
	throw std::runtime_error("Work in progress...");
}

Response	Connection::handleDeleteRequest(const Request& request)
{
	(void) request;
	throw std::runtime_error("Work in progress...");
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
