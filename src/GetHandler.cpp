/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 14:40:03 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/12 19:46:24 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GetHandler.hpp"

GetHandler::GetHandler()
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
	std::string path = sanitizePath(request.getRequestTarget());
	Log::debug("PATH ----> " + path);

	// does the path resource exist
	if (resourceExist(path))
	{
		// fetch content
		response.addHeader("Content-Type", HTTP::getMimeType(path));
		response.setBody(fetchContent(path));
	}
	else
	{
		createErrorResponse(response, WSSC_NOT_FOUND);
	}

	return (response);
}


/* ************************************************************************** */
/* ************************************************************************** */


std::string	GetHandler::sanitizePath(const std::string& request_target)
{
	std::string	path = request_target;
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

std::string	GetHandler::fetchContent(const std::string& path)
{
	std::ifstream	ifs(path, std::ios::binary);

	ifs.open(path);
	if (!ifs.is_open())
		return ("");

	// here we need the parenthesis to tell the compile we want to declare a variable
	// and not a function declaration. Yeah C++, we always love you
	std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	ifs.close();
	
	return (content);
}
