/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 14:40:03 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/11 13:00:54 by nmihaile         ###   ########.fr       */
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

	if (createErrorResponse(response, request.status_code))
		return (response);

	// TODO: restructure:
	// sanitize path
	// does the path resource exist
	// fetch content
	// set headers: Content-Type Content-Length (Http::getMimeType)

	createResponseData(response.data, request);
	return (response);
}


/* ************************************************************************** */
/* ************************************************************************** */


void	GetHandler::createResponseData(std::string& _data, const Request& request)
{
	_data.append(statusLine(request));
	_data.append(headers(request));

	// Finish Header
	_data.append("\r\n");

	std::string file;
	if (request.request_target == "/")
		file = "index.html";
	else
	{
		file = request.request_target;
		file.erase(0,1);
	}

	std::fstream	fs;
	fs.open(file);
	if (!fs.fail() && fs.is_open())
	{
		std::string line;
		while (std::getline(fs, line))
			_data.append(line);
	}

	// _data = std::string("<HTML><body>\n") + std::regex_replace(request.getRequest(), std::regex("\n"), "<br>") + "\n</body></HTML>\0";
}

std::string	GetHandler::statusLine(const Request& request)
{
	std::stringstream status_line;

	status_line	<< "HTTP/1.1 "
				<< std::to_string(request.status_code) << " "
				<< HTTP::getStatusMessage(request.status_code)
				<< "\r\n";
	return (status_line.str());
}

std::string	GetHandler::headers(const Request& request)
{
	std::stringstream headers;

	headers	<< "Content-Type: "
			<< HTTP::getMimeType(request.request_target) << "\n";

	if (endsWith(request.request_target, ".jpg"))
	{
		std::string file(request.request_target);
		file.erase(0,1);

		headers	<< "Content-Length: "
				<< std::to_string(std::__fs::filesystem::file_size(file)) << "\n";
	}

	return (headers.str());
}
