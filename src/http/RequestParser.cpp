/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 14:13:19 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/23 19:00:55 by nmihaile         ###   ########.fr       */
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


void	RequestParser::append(const char *buf, const size_t bytesRead)
{
	if (bytesRead > 0)
		m_rawRequest.append(buf, bytesRead);
	parseNext();
	if (bytesRead == 0 && m_state != State::COMPLETE)
		m_state = State::COMPLETE;
}

void	RequestParser::reset(void)
{
	//	TODO
	//	Reset: RequestParser and Request
	request = Request();
}

void	RequestParser::setError(int statusCode)
{
	// would here a good practice be to use assert
	// since statusCode should here never be below 400
	// like assert(statusCode < 400)
	assert(statusCode >= 400);		//	TODO: delete

	if (!m_error) {
		m_error = true;
		if (request.m_statusCode < WSSC_BAD_REQUEST)
			request.m_statusCode = statusCode;
	}
}

bool	RequestParser::error(void)
{
	return m_error;
}

bool	RequestParser::complete(void)
{
	return m_state == State::COMPLETE;
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
	std::istringstream lineStream(m_rawRequest.substr(0, pos).c_str());
	std::string methodStr;
	if (!std::getline(lineStream, methodStr, ' ') ||
	    !std::getline(lineStream, request.m_requestTarget, ' ') ||
		!std::getline(lineStream, request.m_protokoll, ' ')) {
		request.m_statusCode = WSSC_INTERNAL_SERVER_ERROR;
		m_error = true;
		return;
	}

	if (!validateHttpMethod(methodStr)) {
		if (request.m_statusCode == WSSC_OK) request.m_statusCode = WSSC_INTERNAL_SERVER_ERROR;	//	is this overkill -> unnecessary check?
		m_error = true;
		return;
	}

	if (!validateRequestTarget()) {
		if (request.m_statusCode == WSSC_OK) request.m_statusCode = WSSC_INTERNAL_SERVER_ERROR;	//	is this overkill -> unnecessary check?
		m_error = true;
		return;
	}

	if (!validateProtokoll()) {
		if (request.m_statusCode == WSSC_OK) request.m_statusCode = WSSC_INTERNAL_SERVER_ERROR;	//	is this overkill -> unnecessary check?
		m_error = true;
		return;
	}

	LOG_DEBUG(std::string("parseRequestLine: ") + LIGHTRED + HTTP::methodToString(request.m_method) + " " + LIGHTGREEN + request.m_requestTarget + " " + LIGHTBLUE + request.m_protokoll);

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
		Utils::trimWhitespaces(line);

		if (line.empty()) {
			// TODO: set STATE to rading body and read body
			m_state = State::COMPLETE;
			m_rawRequest.clear();
			return;
		}

		std::pair<std::string, std::string> headerField;
		if (splitLine(line, ':', headerField))
			throw( std::runtime_error("Error: Request::splitLine() could not find delimiter ':'") ); // TODO: better error handling
		request.m_headers[headerField.first] = headerField.second;

		last_pos = pos;
		start = pos + 1;
		pos = m_rawRequest.find_first_of("\n", start);
	}
	if (last_pos != std::string::npos)
		m_rawRequest.erase(0, last_pos + 1);
}

bool	RequestParser::validateHttpMethod(std::string& methodStr)
{
	if (Validator::isValidMethod(methodStr, request.m_method))
		return true;

	std::string ustr = methodStr;
	std::transform(ustr.begin(), ustr.end(), ustr.begin(), [](char c){
		return (std::toupper(c));
	});

	if (ustr == "GET\0" || ustr == "POST\0" || ustr == "DELETE\0") {
		request.m_method = HTTP::strToMethod(methodStr);
		// setStatusCode(_status_code, WSSC_BAD_REQUEST);
		request.m_statusCode = WSSC_BAD_REQUEST;
		return false;
	}
		
	// request.m_method = HTTP::Method::GET;
	// setStatusCode(_status_code, WSSC_METHOD_NOT_ALLOWED);
	request.m_statusCode = WSSC_METHOD_NOT_ALLOWED;
	return false;
}

// void	RequestParser::setStatusCode(int& _status_code, int _new_status)
// {
// 	if (_status_code == 0)
// 		_status_code = _new_status;
// }

bool	RequestParser::validateRequestTarget(void)
{
	if (!percentDecoding(request.m_requestTarget, request.m_URI)) {
		setError(WSSC_BAD_REQUEST);
		return false;
	}
	//	TODO:	sanitize URI

	LOG_INFO_LM("request.m_URI", request.m_URI);

	if (request.m_requestTarget.empty())
		return false;
	// TODO the rest !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	return true;
}

bool	RequestParser::validateProtokoll(void)
{
	if (request.m_protokoll.empty())
		return false;
	// TODO the rest !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	return true;
}

bool	RequestParser::percentDecoding(const std::string& requestTarget, std::string& destURI)
{
	destURI.reserve(requestTarget.size());
	
	for (size_t idx = 0; idx < requestTarget.size(); ++idx) {
		char c = requestTarget[idx];
		if (c == '%') {
			if (idx + 2 < requestTarget.size() &&
			    std::isxdigit(static_cast<unsigned char>(requestTarget[idx + 1])) &&
			    std::isxdigit(static_cast<unsigned char>(requestTarget[idx + 2]))) {
					// Valid percent-encoding: decode and append character
					char hiHex = requestTarget[++idx];
					char loHex = requestTarget[++idx];
					destURI += static_cast<char>((hexToInt(hiHex) << 4) | hexToInt(loHex));
			} else {
				// invalid Hex character, or to short for valid hex form: xFF
				return false;
			}
		} else {
			destURI += c;
		}	
	}
	return true;
}

int	RequestParser::hexToInt(const char c)
{
	return std::isdigit(c) ? c - '0' : std::tolower(c) - 'a' + 10;
}

bool	RequestParser::splitLine(std::string &line, char del, std::pair<std::string, std::string> &headerField)
{
	size_t	pos = line.find(del);
	if (pos == std::string::npos)
		return (true);

	headerField.first = line.substr(0, pos);
	headerField.second = line.substr(pos + 1);
	Utils::trimWhitespaces(headerField.first);
	Utils::trimWhitespaces(headerField.second);

	return (false);
}
