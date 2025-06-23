/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 14:13:19 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/23 09:53:42 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestParser.hpp"

RequestParser::RequestParser()
	:	m_error(false),
		m_state(State::READING_REQUEST_LINE)
{
}

RequestParser::~RequestParser()
{
}


/* ************************************************************************** */
/* ************************************************************************** */


void	RequestParser::append(const char *buf, const size_t bytes_read)
{
	m_rawRequest.append(buf, bytes_read);
	parseNext();
}


/* ************************************************************************** */
/* ************************************************************************** */


void	RequestParser::parseNext(void)
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
}

void	RequestParser::parseRequestLine(void)
{
	LOG_DEBUG("Request::parseRequestLine");

	size_t	pos = m_rawRequest.find("\r\n");
	if (pos == std::string::npos)
		return;

	// read the elements of the requestLine
	std::istringstream lineStream(m_rawRequest.substr(0, pos));
	std::string methodStr;
	if (!std::getline(lineStream, methodStr, ' ') ||
	    !std::getline(lineStream, m_requestTarget, ' ') ||
		!std::getline(lineStream, m_HTTPversion, ' ')) {
		request.m_statusCode = WSSC_INTERNAL_SERVER_ERROR;
		m_state = State::ERROR;
		return;
	}

	if (!validateHttpMethod(methodStr, m_request.m_method, m_request.m_statusCode)) {
		request.m_statusCode = WSSC_INTERNAL_SERVER_ERROR;
		m_state = State::ERROR;
		return;
	}

	//	TODO next: OLD CODE BELOW FROM REQUEST TO ADD
	//	REQUEST_TARGET, PROTOKOLL VALIDATIONS

	// isLine >> m_requestTarget;
	if (isLine.fail()) {
		m_state = State::ERROR;
		m_statusCode = WSSC_INTERNAL_SERVER_ERROR;
		return;
	}

	// isLine >> m_HTTPversion;
	if (isLine.fail()) {
		m_state = State::ERROR;
		m_statusCode = WSSC_INTERNAL_SERVER_ERROR;
		return;
	}

	LOG_DEBUG(std::string("parseRequestLine: ") + LIGHTRED + HTTP::methodToString(m_method) + " " + LIGHTGREEN + m_requestTarget + " " + LIGHTBLUE + m_HTTPversion);

	m_rawRequest.erase(0, pos + 2);
	m_state = State::READING_HEADERS;
}

void	RequestParser::parseHeader(void)
{
	size_t	start = 0;
	size_t	pos, last_pos;

	pos = m_rawRequest.find_first_of('\n');
	last_pos = pos;
	while (pos != std::string::npos) {
		std::string line = m_rawRequest.substr(start, pos - start);
		trimWhitespaces(line);

		if (line.empty()) {
			// TODO: set STATE to rading body and read body
			m_state = State::COMPLETE;
			m_rawRequest.clear();
			return;
		}

		std::pair<std::string, std::string> headerField;
		if (splitLine(line, ':', headerField))
			throw( std::runtime_error("Error: Request::splitLine() could not find delimiter ':'") ); // TODO: better error handling
		m_headers[headerField.first] = headerField.second;

		last_pos = pos;
		start = pos + 1;
		pos = m_rawRequest.find_first_of("\n", start);
	}
	if (last_pos != std::string::npos)
		m_rawRequest.erase(0, last_pos + 1);
}

bool	RequestParser::validateHttpMethod(std::string& methodStr, HTTP::Method& dest, int& statusCode)
{
	if (Validator::isValidMethod(methodStr, dest))
		return true;

	std::string ustr = methodStr;
	std::transform(ustr.begin(), ustr.end(), ustr.begin(), [](char c){
		return (std::toupper(c));
	});

	if (ustr == "GET\0" || ustr == "POST\0" || ustr == "DELETE\0") {
		dest = HTTP::strToMethod(methodStr);
		// setStatusCode(_status_code, WSSC_BAD_REQUEST);
		statusCode = WSSC_BAD_REQUEST;
		return false;
	}
		
	dest = HTTP::Method::GET;
	// setStatusCode(_status_code, WSSC_METHOD_NOT_ALLOWED);
	statusCode = WSSC_METHOD_NOT_ALLOWED;
	return false;
}

// void	RequestParser::setStatusCode(int& _status_code, int _new_status)
// {
// 	if (_status_code == 0)
// 		_status_code = _new_status;
// }

bool	RequestParser::splitLine(std::string &line, char del, std::pair<std::string, std::string> &headerField)
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
