/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tunsal <tunsal@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 18:06:22 by nmihaile          #+#    #+#             */
/*   Updated: 2025/07/01 19:16:37 by tunsal           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include <algorithm>

Request::Request(const ServerBlock& _serverBlock)
	:	parsingState(ParsingState::READING_REQUEST_LINE),
		method(HTTP::Method::GET),
		statusCode(200),
		serverBlock(_serverBlock),
		m_error(false),
		m_locationBlock(nullptr)
{
}

Request::~Request()
{
}

/* ************************************************************************** */
/* ************************************************************************** */

void	Request::setError(int errorStatusCode)
{
	if (!m_error) {
		m_error = true;
		if (statusCode < WSSC_BAD_REQUEST)
			statusCode = errorStatusCode;
	}
}

bool	Request::error(void)
{
	return m_error;
}

int	Request::getStatusCode(void) const
{
	return (statusCode);
}

std::string	Request::getRequestTarget(void) const
{
	return (requestTarget);
}

const LocationBlock&	Request::locationBlock()
{
	if (m_locationBlock == nullptr) {
		// Matches the requestTaget to a serverBlock.locationBlock
		// TODO:	more needs o be done
		for (const LocationBlock& locationBlock : serverBlock.locationBlocks) {
			if (locationBlock.route == URI) {
				m_locationBlock = const_cast<LocationBlock*>(&locationBlock);
				return *m_locationBlock;
			}
		}
		m_locationBlock = const_cast<LocationBlock*>(&serverBlock.locationBlocks.front());
	}
	return *m_locationBlock;
}

bool	Request::isAllowedMethod(void)
{
	//	TODO:	should we protect this methode here and only accept it if the
	//			request is complete or has an error????
	return std::find(locationBlock().allowMethods.begin(), locationBlock().allowMethods.end(), method) != locationBlock().allowMethods.end();
}
