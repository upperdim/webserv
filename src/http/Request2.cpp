/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request2.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 18:06:22 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/24 18:19:37 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request2.hpp"

Request::Request()
	:	method(HTTP::Method::GET),
		statusCode(200),
		m_state(State::READING_REQUEST_LINE),
		m_error(false)
{
}

Request::~Request()
{
}

// move assignment operator
Request&	Request::operator=(Request&& rhs)
{
	if (this != &rhs) {
		rawRequest		= std::move(rhs.rawRequest);
		method			= rhs.method;
		statusCode		= rhs.statusCode;
		requestTarget	= std::move(rhs.requestTarget);
		URI				= std::move(rhs.URI);
		protokoll		= std::move(rhs.protokoll);
		headers			= std::move(rhs.headers);
		m_state			= rhs.m_state;
		m_error			= rhs.m_error;
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

void	Request::setError(int _statusCode)
{
	if (!m_error) {
		m_error = true;
		if (statusCode < WSSC_BAD_REQUEST)
			statusCode = _statusCode;
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

const LocationBlock&	Request::getLocation(const ServerBlock& serverBlock) const
{
	// Matches the requestTaget to a serverBlock.locationBlock
	// TODO:	more needs o be done
	for (const auto& locationBlock : serverBlock.locationBlocks) {
		if (requestTarget == locationBlock.route)
			return locationBlock;
	}
	return serverBlock.locationBlocks.front();
}
