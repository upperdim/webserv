/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 18:06:22 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/29 21:27:02 by nmihaile         ###   ########.fr       */
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
	statusCode = errorStatusCode;
	m_state = State::INVALID;
	isComplete = true;
}

// I added this function so it can be called from clientConnection when
// (recv == 0)
// it made more sense to call setComplete() there instead of calling
// resolveRequestContext()
// I am intressted on your thoughts…
void	Request::setComplete(void)
{
	if (m_state != State::INVALID)
		m_state = State::COMPLETE;
	isComplete = true;
	resolveRequestContext();
}

void	Request::resolveRequestContext(void)
{
	//	INFO:	I call this func currently resolveRequestContext(), since it is
	//			called in RequesParser. If we move the RequestParser
	//			into request this might be obsolete, or we do sth complete
	//			different which depends on your ideas

	// REQUEST RESOLVE PROCESSING FLOW (see WSV-42 bottom)
	// ===============================
	// use Host from header to choose ServerBlock.
	// resolve serverBlock
	// resolve location via URI
	// resolve root
	// resolve path
	// resolve clientMaxBodySize
	// resolve index

	m_state = State::COMPLETE;
	isComplete = true;
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
