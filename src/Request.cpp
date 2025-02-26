/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/25 17:46:49 by nmihaile          #+#    #+#             */
/*   Updated: 2025/02/26 18:39:46 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request()
	:	state(State::READING_REQUEST_LINE)
{
}

Request::~Request()
{
}


/* ************************************************************************** */
/* ************************************************************************** */


void	Request::append(char buf[REQUEST_BUFFER_SIZE], size_t bytes_read)
{
	m_raw_request.append(buf, bytes_read);
	parseNext();
}


/* ************************************************************************** */
/* ************************************************************************** */


void	Request::parseNext(void)
{
	Log::debug("Request::parseNext");
	if (state == State::READING_REQUEST_LINE) {
		Log::debug("===> READING_REQUEST_LINE");
		parseRequestLine();
	}
	if (state == State::READING_HEADERS) {
		Log::debug("===> READING_HEADERS");
	}
	if (state == State::READING_BODY) {
		Log::debug("===> READING_BODY");
	}
	if (state == State::COMPLETE) {
		Log::debug("===> COMPLETE");
	}
	if (state == State::ERROR) {
		Log::debug("===> ERROR");
	}
}

void	Request::parseRequestLine(void)
{
	Log::debug("Request::parseRequestLine");

	size_t	pos = m_raw_request.find("\r\n");
	if (pos == std::string::npos)
		return ;

	std::stringstream ss(m_raw_request);

	ss >> m_method;
	if (ss.fail())
		state = State::ERROR;

	ss >> m_request_target;
	if (ss.fail())
		state = State::ERROR;

	ss >> m_HTTP_version;
	if (ss.fail())
		state = State::ERROR;

	Log::debug(std::string("parseRequestLine: ") + LIGHTRED + m_method + " " + LIGHTGREEN + m_request_target + " " + LIGHTYELLOW + m_HTTP_version);

	m_raw_request.erase(0, pos + 2);
	state = State::READING_HEADERS;

	Log::msg("CAPED REQUEST:\n", m_raw_request, LIGHTYELLOW, LIGHTCYAN);
}
