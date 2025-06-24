/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request2.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 18:06:22 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/24 17:45:21 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request2.hpp"

Request::Request()
	:	method(HTTP::Method::GET),
		m_state(State::READING_REQUEST_LINE),
		m_error(false),
		m_statusCode(200)		
{
}

Request::~Request()
{
}

// move assignment operator
Request&	Request::operator=(Request&& rhs)
{
	if (this != &rhs) {
		m_statusCode	= rhs.m_statusCode;
		m_requestTarget	= std::move(rhs.m_requestTarget);
		m_protokoll		= std::move(rhs.m_protokoll);
		method			= rhs.method;
		m_headers		= std::move(rhs.m_headers);
	}
	return *this;
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

void	Request::setError(int statusCode)
{
	if (!m_error) {
		m_error = true;
		if (request.m_statusCode < WSSC_BAD_REQUEST)
			request.m_statusCode = statusCode;
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
	return (m_statusCode);
}

std::string	Request::getRequestTarget(void) const
{
	return (m_requestTarget);
}

const LocationBlock&	Request::getLocation(const ServerBlock& serverBlock) const
{
	// Matches the requestTaget to a serverBlock.locationBlock
	// TODO:	more needs o be done
	for (const auto& locationBlock : serverBlock.locationBlocks) {
		if (m_requestTarget == locationBlock.route)
			return locationBlock;
	}
	return serverBlock.locationBlocks.front();
}
