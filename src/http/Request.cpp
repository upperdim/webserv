/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tunsal <tunsal@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/25 17:46:49 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/18 22:26:30 by tunsal           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request()
	:	m_state(State::READING_REQUEST_LINE),
		m_status_code(200),
		m_method(HTTP::Method::GET)
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
	if (m_state == State::COMPLETE)
		return (true);
	return (false);
}

int	Request::error(void) const
{
	if (m_state == State::ERROR)
		return (m_status_code);
	return (0);
}

std::string	Request::getRequest(void) const
{
	std::string req = getRequestLine();
	for (auto it = m_headers.begin(); it != m_headers.end(); ++it)
		req += "\n" + it->first + ": " + it->second;
	return (req);
}

std::string	Request::getRequestLine(void) const
{
	std::string	req = HTTP::methodToString(m_method) + " "
					+ m_request_target
					+ " " + m_HTTP_version;
	return (req);
}

int	Request::getStatusCode(void) const
{
	return (m_status_code);
}

HTTP::Method	Request::getMethod(void) const
{
	return (m_method);
}

std::string	Request::getRequestTarget(void) const
{
	return (m_request_target);
}

void	Request::setComplete()
{
	if (m_state < State::COMPLETE)
		m_state = State::COMPLETE;
}

void	Request::setError(int _status_code)
{
	m_state = State::ERROR;
	m_status_code = _status_code;
}


/* ************************************************************************** */
/* ************************************************************************** */


void	Request::parseNext(void)
{
	LOG_DEBUG("Request::parseNext");
	if (m_state == State::READING_REQUEST_LINE) {
		LOG_DEBUG("===> REQUEST READING_REQUEST_LINE");
		parseRequestLine();
	}
	if (m_state == State::READING_HEADERS) {
		LOG_DEBUG("===> REQUEST READING_HEADERS");
		parseHeader();
	}
	if (m_state == State::READING_BODY) {
		LOG_DEBUG("===> REQUEST READING_BODY");
	}
	if (m_state == State::COMPLETE) {
		LOG_DEBUG("===> REQUEST COMPLETE");
	}
	if (m_state == State::ERROR) {
		LOG_DEBUG("===> ERROR");
	}
}

void	Request::parseRequestLine(void)
{
	LOG_DEBUG("Request::parseRequestLine");

	size_t	pos = m_raw_request.find("\r\n");
	if (pos == std::string::npos)
		return ;

	std::stringstream ss(m_raw_request);

	std::string methodStr;
	ss >> methodStr;
	if (Validate::sstream(ss.fail(), m_status_code) || !Validate::validateHttpMethod(methodStr, m_method, m_status_code))
		m_state = State::ERROR;

	ss >> m_request_target;
	if (Validate::sstream(ss.fail(), m_status_code))
		m_state = State::ERROR;

	ss >> m_HTTP_version;
	if (Validate::sstream(ss.fail(), m_status_code))
		m_state = State::ERROR;

	LOG_DEBUG(std::string("parseRequestLine: ") + LIGHTRED + HTTP::methodToString(m_method) + " " + LIGHTGREEN + m_request_target + " " + LIGHTBLUE + m_HTTP_version);

	m_raw_request.erase(0, pos + 2);
	m_state = State::READING_HEADERS;
}

void	Request::parseHeader(void)
{
	size_t	start = 0;
	size_t	pos, last_pos;

	pos = m_raw_request.find_first_of('\n');
	last_pos = pos;
	while (pos != std::string::npos)
	{
		std::string line = m_raw_request.substr(start, pos - start);
		trimWhitespaces(line);

		if (line.empty())
		{
			// TODO: set STATE to rading body and read body
			m_state = State::COMPLETE;
			m_raw_request.clear();
			return;
		}

		std::pair<std::string, std::string> headerField;
		if (splitLine(line, ':', headerField))
			throw( std::runtime_error("Error: Request::splitLine() could not find delimiter ':'") ); // TODO: better error handling
		m_headers[headerField.first] = headerField.second;

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

	return (false);
}
