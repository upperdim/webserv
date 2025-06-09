/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 14:40:03 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/21 12:53:18 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GetHandler.hpp"

GetHandler::GetHandler(const Server& _server) : AHandler(_server)
{
}

GetHandler::~GetHandler()
{
}


/* ************************************************************************** */
/* ************************************************************************** */


Response	GetHandler::handle(const Request& request)
{
	Response response;

	if (request.error())
	{
		createErrorResponse(response, request.getStatusCode());
		return (response);
	}

	// TODO: restructure:
	// sanitize path
	std::string path = sanitizePath(request, response);
	LOG_DEBUG("PATH ----> " + path);

	// does the path resource exist
	if (resourceExist(path))
	{
		// fetch content
		response.addHeader("Content-Type", HTTP::getMimeType(path));
		response.setBodyFileBufferReader(path);
	}
	else
		createErrorResponse(response, WSSC_I_M_A_TEAPOT);

	return (response);
}


/* ************************************************************************** */
/* ************************************************************************** */


std::string	GetHandler::sanitizePath(const Request& request, Response& response)
{
	std::string	root_path	= m_server.getRootDir();
	std::string path		= request.getRequestTarget();

	(void)response;	// TODO: delete me

	if (path == "/")
		path += "index.html";
	while (startsWith(path, "../"))
		path.erase(0, 3);
	while (startsWith(path, "/"))
		path.erase(0, 1);
	return (path);
}

bool	GetHandler::resourceExist(const std::string& path)
{
	std::ifstream	file(path);
	return ( file.good() );
}
