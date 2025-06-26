/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPMethodHandler.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/26 11:01:47 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/26 17:01:06 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPMethodHandler.hpp"
#include "Utils.hpp"

HTTPMethodHandler::~HTTPMethodHandler()
{
}

/* ************************************************************************** */
/* ************************************************************************** */

//	passing the response as argument reduces the count we assign it
//	HTTP::handle() could also return the response, but I went with argument first
void	HTTPMethodHandler::handle(Request& request, Response& response)
{
	if (request.error()) {
		handleFailedRequest(request, response);
	} else {
		if (request.getRequestTarget().empty()) {
			response.setStatus(WSSC_NOT_FOUND);
		} else if (!request.isAllowedMethod()) {
			response.setStatus(WSSC_METHOD_NOT_ALLOWED);
		} else {
			switch (request.method) {
				case HTTP::Method::GET:
					handleGetRequest(request, response);
					break;
				case HTTP::Method::POST:
					handlePostRequest(request, response);
					break;
				case HTTP::Method::DELETE:
					handleDeleteRequest(request, response);
					break;
			}
		}
	}
}


/* ************************************************************************** */
/* ************************************************************************** */


void	HTTPMethodHandler::handleGetRequest(const Request& request, Response& response)
{
	LOG_MSG("[handle Get Request] ", "...", LIGHTMAGENTA, DEFAULT);

	// TODO: repetitive?
	if (request.getStatusCode() >= WSSC_BAD_REQUEST) {
		createErrorResponse(response, request.getStatusCode());
		return;
	}

	// TODO: restructure:
	// sanitize path
	std::string path = Utils::sanitizePath(request, request.serverBlock);
	LOG_DEBUG("PATH ----> " + path);

	// does the path resource exist
	if (Utils::resourceExist(path)) {
		// fetch content
		response.addHeader("Content-Type", HTTP::getMimeType(path));
		response.setBodyFileBufferReader(path);
	}
	else
		createErrorResponse(response, WSSC_I_M_A_TEAPOT);

	return;
}

void	HTTPMethodHandler::handlePostRequest(const Request& request, Response& response)
{
	LOG_MSG("[handle Post Request] ", "...", LIGHTMAGENTA, DEFAULT);
	(void) request;
	(void) response;
	throw std::runtime_error("Work in progress...");
}

void	HTTPMethodHandler::handleDeleteRequest(const Request& request, Response& response)
{
	LOG_MSG("[handle DELETE Request] ", "...", LIGHTMAGENTA, DEFAULT);

	std::filesystem::path resourcePath(Utils::sanitizePath(request, request.serverBlock));
	if (!(std::filesystem::exists(resourcePath) && std::filesystem::is_regular_file(resourcePath))) {
		// resourcePath NOT FOUND
		response.setStatus(WSSC_NOT_FOUND);
		return;
	}

	LOG_WARNING_LM("DELETING", resourcePath.c_str());
	if (!std::filesystem::remove(resourcePath)) {
		// failed to remove
		response.setStatus(WSSC_INTERNAL_SERVER_ERROR);
		return;
	}
	LOG_SUCCESS(std::string("deleted: ") + resourcePath.c_str());

	response.setStatus(WSSC_OK);
	return;
}

void	HTTPMethodHandler::handleFailedRequest(const Request& request, Response& response)
{
	LOG_MSG("[handle failed request] ", "...", LIGHTMAGENTA, DEFAULT);

	int			status_code = request.getStatusCode();

	if (status_code < WSSC_BAD_REQUEST)
		status_code = WSSC_INTERNAL_SERVER_ERROR;

	createErrorResponse(response, status_code);
	return;
}

void	HTTPMethodHandler::createErrorResponse(Response& response, int statusCode)
{
	//	TODO:	reexamen this will we need it like that or can we reduce
	//			the statusCode for exameple etc…
	response.setProtokoll("HTTP/1.1");
	response.setStatus(statusCode);
	response.addHeader("Content-Type", HTTP::getMimeType(".html"));
	response.setBodyString(HTTP::getErrorPageTemplate(statusCode));
}
