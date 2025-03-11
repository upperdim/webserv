/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/25 17:46:49 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/10 12:49:06 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request()
	: state(State::READING_REQUEST_LINE),
	  status_code(200)
{
}

Request::~Request()
{
}


/* ************************************************************************** */
/* ************************************************************************** */


void	Request::append(char buf[REQUEST_BUFFER_SIZE], size_t bytes_read)
{
	raw_request.append(buf, bytes_read);
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
		return (status_code);
	return (0);
}

std::string	Request::getRequest(void) const
{
	std::string req = getRequestLine();
	for (auto it = headers.begin(); it != headers.end(); ++it)
		req += "\n" + it->first + ": " + it->second ;
	return (req);
}

std::string	Request::getRequestLine(void) const
{
	std::string	req = method + " " + request_target + " " + HTTP_version;
	return (req);
}

// std::string	Request::getMethod(void) const
// {
// 	return (method);
// }

// std::string	Request::getRequestTarget(void) const
// {
// 	return (request_target);
// }

void	Request::setComplete()
{
	if (state < State::COMPLETE)
		state = State::COMPLETE;
}

void	Request::setError(State _state, int _status_code)
{
	state = _state;
	status_code = _status_code;
}


/* ************************************************************************** */
/* ************************************************************************** */


void	Request::parseNext(void)
{
	Log::debug("Request::parseNext");
	if (state == State::READING_REQUEST_LINE) {
		Log::debug("===> REQUEST READING_REQUEST_LINE");
		parseRequestLine();
	}
	if (state == State::READING_HEADERS) {
		Log::debug("===> REQUEST READING_HEADERS");
		parseHeader();
	}
	if (state == State::READING_BODY) {
		Log::debug("===> REQUEST READING_BODY");
	}
	if (state == State::COMPLETE) {
		Log::debug("===> REQUEST COMPLETE");
	}
	if (state == State::ERROR) {
		Log::debug("===> ERROR");
	}
}

void	Request::parseRequestLine(void)
{
	Log::debug("Request::parseRequestLine");

	size_t	pos = raw_request.find("\r\n");
	if (pos == std::string::npos)
		return ;

	std::stringstream ss(raw_request);

	ss >> method;
	if (Validate::sstream(ss.fail(), status_code) || Validate::method(method, status_code))
		state = State::ERROR;

	ss >> request_target;
	if (Validate::sstream(ss.fail(), status_code))
		state = State::ERROR;

	ss >> HTTP_version;
	if (Validate::sstream(ss.fail(), status_code))
		state = State::ERROR;

	Log::debug(std::string("parseRequestLine: ") + LIGHTRED + method + " " + LIGHTGREEN + request_target + " " + LIGHTBLUE + HTTP_version);

	raw_request.erase(0, pos + 2);
	state = State::READING_HEADERS;

	// Log::msg("CAPPED REQUEST aka HEADERS:\n", raw_request, LIGHTYELLOW, LIGHTCYAN);
}

void	Request::parseHeader(void)
{
	size_t	start = 0;
	size_t	pos, last_pos;

	pos = raw_request.find_first_of('\n');
	last_pos = pos;
	while (pos != std::string::npos)
	{
		std::string line = raw_request.substr(start, pos - start);
		trimWhitespaces(line);

		if (line.empty())
		{
			// TODO: set STATE to rading body and read body
			state = State::COMPLETE;
			raw_request.clear();
			return;
		}

		std::pair<std::string, std::string> headerField;
		if (splitLine(line, ':', headerField))
			throw( std::runtime_error("Error: Request::splitLine() could not find delimiter ':'") ); // TODO: better error handling
		headers[headerField.first] = headerField.second;

		last_pos = pos;
		start = pos + 1;
		pos = raw_request.find_first_of("\n", start);
	}
	if (last_pos != std::string::npos)
		raw_request.erase(0, last_pos + 1);
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
