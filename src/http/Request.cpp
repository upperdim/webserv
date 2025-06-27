/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 18:06:22 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/25 11:27:32 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include <algorithm>

Request::Request(const ServerBlock& _serverBlock)
	:	isComplete(false),
		method(HTTP::Method::GET),
		statusCode(200),
		serverBlock(_serverBlock),
		m_state(State::READING_REQUEST_LINE),
		m_error(false),
		m_locationBlock(nullptr)
{
}

Request::~Request()
{
}

/* ************************************************************************** */
/* ************************************************************************** */


void	Request::append(const char *buf, const size_t bytesRead)
{
	rawRequest.append(buf, bytesRead);
}

void	Request::reset(void)
{
	//	TODO:	reset the Request
}

Request::State	Request::getState(void)
{
	return m_state;
}

void	Request::setState(State state)
{
	m_state = state;
}

void	Request::setError(int errorStatusCode)
{
	if (!m_error) {
		m_error = true;
		if (statusCode < WSSC_BAD_REQUEST)
			statusCode = errorStatusCode;
	}
}

void	Request::setComplete()
{
	m_state = State::COMPLETE;
}

bool	Request::error(void)
{
	return m_error;
}

bool	Request::complete(void)
{
	return m_state == State::COMPLETE;
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
