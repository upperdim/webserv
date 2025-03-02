/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/25 17:46:49 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/02 18:33:46 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request()
	: state(State::READING_REQUEST_LINE),
	  m_error_code(0)
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

bool	Request::complete(void) const
{
	if (state == State::COMPLETE)
		return (true);
	return (false);
}

int	Request::error(void) const
{
	if (state == State::ERROR)
		return (m_error_code);
	return (0);
}

std::string	Request::getRequest(void) const
{
	std::string req = getRequestLine();
	for (auto it = m_headers.begin(); it != m_headers.end(); ++it)
		req += "\n" + it->first + ": " + it->second ;
	return (req);
}

std::string	Request::getRequestLine(void) const
{
	std::string	req = m_method + " " + m_request_target + " " + m_HTTP_version;
	return (req);
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
		parseHeader();
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

	// Log::msg("CAPPED REQUEST aka HEADERS:\n", m_raw_request, LIGHTYELLOW, LIGHTCYAN);
}

void	Request::parseHeader(void)
{
	size_t	start = 0;
	size_t	pos, last_pos;
	size_t	header_end = m_raw_request.find("\r\n\r\n");

	pos = m_raw_request.find_first_of('\n');
	last_pos = pos;
	while (pos != std::string::npos)
	{
		std::string line = m_raw_request.substr(start, pos - start);
		trimWhitespaces(line);

		std::pair<std::string, std::string> headerField;
		if (splitLine(line, ':', headerField))
			throw( std::runtime_error("Error: Request::splitLine() could not find delimiter ':'") ); // TODO: better error handling
			
		m_headers[headerField.first] = headerField.second;
			
		if (pos >= header_end)
		{
			Log::msg("DONE\n", "[------------------------------------------------------------------------------]", LIGHTRED, RED);
			last_pos = m_raw_request.length() - 1;
			// TODO: set STATE to rading body and read body
			state = State::COMPLETE;
			break;
		}

		last_pos = pos;
		start = pos + 1;
		pos = m_raw_request.find_first_of("\n", start);
	}
	if (last_pos != std::string::npos)
		m_raw_request.erase(0, last_pos + 1);
}

bool	Request::splitLine(std::string &line, char del, std::pair<std::string, std::string> &headerField)
{
	size_t	pos = line.find(del);
	if (pos == std::string::npos)
		return (true);

	headerField.first = line.substr(0, pos);
	headerField.second = line.substr(pos + 1);
	trimWhitespaces(headerField.first);
	trimWhitespaces(headerField.second);

	// Log::msg("headerField [key|value] => ", "[" + headerField.first + "|" + headerField.second + "]", LIGHTCYAN, DEFAULT);
	return (false);
}
