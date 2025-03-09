/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 14:40:03 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/09 18:10:16 by nmihaile         ###   ########.fr       */
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

	createResponseData(response.data, request);
	return (response);
}


/* ************************************************************************** */
/* ************************************************************************** */


void	GetHandler::createResponseData(std::string& _data, const Request& request)
{
	_data.append(statusLine(request));
	_data.append("\r\n");

	std::fstream	fs;
	fs.open("index.html");
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
	std::string status_line;
	status_line += "HTTP/1.1 ";
	status_line += std::to_string(request.status_code);
	status_line += " ";
	status_line += HTTP::getStatusMessage(request.status_code);
	status_line += "\r\n";
	return (status_line);
}
