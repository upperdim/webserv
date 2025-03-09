/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AHandler.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 17:47:24 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/09 18:09:27 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AHandler.hpp"

AHandler::~AHandler()
{
}


/* ************************************************************************** */
/* ************************************************************************** */


bool	AHandler::createErrorResponse(Response& response, int _status_code)
{
	if (_status_code >= 400)
	{
		response.data += "HTTP/1.1 ";
		response.data += std::to_string(_status_code);
		response.data += " ";
		response.data += HTTP::getStatusMessage(_status_code);
		response.data += "\r\n";
		return (true);
	}

	return (false);
}
