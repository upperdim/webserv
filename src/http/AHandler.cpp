/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AHandler.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 17:47:24 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/18 14:38:33 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AHandler.hpp"

AHandler::AHandler(const ServerBlock& _serverBlock)
	:	m_serverBlock(_serverBlock)
{
}

AHandler::~AHandler()
{
}


/* ************************************************************************** */
/* ************************************************************************** */


void	AHandler::createErrorResponse(Response& response, int _status_code)
{
	response.setProtokoll("HTTP/1.1");
	response.setStatus(_status_code, HTTP::getStatusMessage(_status_code));
	response.addHeader("Content-Type", HTTP::getMimeType(".html"));
	response.setBodyString(fetchErrorPage(_status_code));
}

std::string	AHandler::fetchErrorPage(int _status_code) const
{
	return (HTTP::getErrorPageTemplate(_status_code));
}
