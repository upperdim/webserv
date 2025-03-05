/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 11:29:39 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/05 12:03:02 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response()
{
}

Response::~Response()
{
}


/* ************************************************************************** */
/* ************************************************************************** */


void	Response::create(const Request& request)
{
	data.append(statusLine(request));
	data.append("\r\n");
	data = std::string("<HTML><body>\n") + std::regex_replace(request.getRequest(), std::regex("\n"), "<br>") + "\n</body></HTML>\0";
}


/* ************************************************************************** */
/* ************************************************************************** */


std::string	Response::statusLine(const Request& request)
{
	std::string status_line;
	status_line += "HTTP/1.1 ";
	status_line += std::to_string(request.status_code);
	status_line += " ";
	status_line += HTTP::reason_phrase.at(request.status_code);
	status_line += "\r\n";
	return (status_line);
}
