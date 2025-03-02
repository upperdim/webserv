/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/25 17:46:49 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/02 14:49:03 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request()
	:	state(State::READING_REQUEST_LINE),
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

bool	Request::complete(void)
{
	if (state == State::COMPLETE)
		return (true);
	return (false);
}

int		Request::error(void)
{
	if (state == State::ERROR)
		return (m_error_code);
	return (0);
}

std::string	Request::getRequest(void)
{
	std::string req = getRequestLine() + "\n";

	for (auto it = m_headers.begin(); it != m_headers.end(); ++it)
		req += it->first + ": " + it->second + "\n";

	return (req);
}

std::string	Request::getRequestLine(void)
{
	std::string req;
	
	req += m_method	+ " ";
	req += m_request_target + " ";
	req += m_HTTP_version;

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

	Log::msg("CAPED REQUEST:\n", m_raw_request, LIGHTYELLOW, LIGHTCYAN);
}

void	Request::parseHeader(void)
{
	size_t start = 0;
	size_t pos;
	size_t header_end = m_raw_request.find("\r\n\r\n");

	if (header_end != std::string::npos)
	{
		Log::success("----------- HEADER LOEDED COMPLETELY ----------------");
		Log::msg("",m_raw_request, YELLOW, YELLOW);
	}

	pos = m_raw_request.find_first_of('\n');
	while (pos != std::string::npos)
	{
		std::string line;
		std::pair<std::string, std::string>	headerField;
		line = m_raw_request.substr(start, pos - start);
		trimWhitespaces(line);
		Log::msg("-----LINE------\n", std::string(line) + "\033[0m|<<<<<<<", RED, BLUE);


		// if (line.find("\r\n\r\n") != std::string::npos)
		// {
		// 	Log::msg("Ups", std::string("[") + m_raw_request + "]", DEFAULT, YELLOW);
		// 	return ;
		// }

		if (splitLine(line, headerField))
			throw ( std::runtime_error("Upsi: split header line went wrong") );	// TODO: better error handling

		if (pos >= header_end)
		{
			Log::msg("DONE\n", "[--------------------------------------------------------------------]", LIGHTRED, RED);
			// Log::msg("Ups\n", std::string("[") + m_raw_request + "]", LIGHTRED, RED);
			pos = m_raw_request.length() - 1;
			state = State::COMPLETE;
			break ;
		}

		start = pos + 1;
		pos = m_raw_request.find_first_of("\n", start);
	}
	m_raw_request.erase(0, pos + 1);
}

bool	Request::splitLine(std::string& line, std::pair<std::string, std::string>& headerField)
{
	size_t pos = line.find(':');
	if (pos == std::string::npos)
		return (true);

	headerField.first	= line.substr(0, pos) ;
	headerField.second	= line.substr(pos + 1);
	trimWhitespaces(headerField.first) ;
	trimWhitespaces(headerField.second);
	
	Log::msg("headerField [key|value] => ", "[" + headerField.first + "|" + headerField.second + "]", LIGHTCYAN, DEFAULT);

	return (false);
}
