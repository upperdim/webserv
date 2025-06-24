/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 14:13:19 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/24 16:48:06 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestParser.hpp"
#include <sstream>
#include "HTTP.hpp"
#include "Validator.hpp"
#include "Log.hpp"
#include "Utils.hpp"

RequestParser::~RequestParser()
{
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


/* ************************************************************************** */
/* ************************************************************************** */


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
		setError(WSSC_INTERNAL_SERVER_ERROR);
		return;
	}

	// Don't allow extra garbage after the protokoll in the request line
	std::string someExtraGarbage;
	if (lineStream >> someExtraGarbage) {
		setError(WSSC_BAD_REQUEST);
		return;
	}

	LOG_DEBUG(std::string("RAW       ~~ RequestLine: ") + LIGHTRED + HTTP::methodToString(request.m_method) + " " + LIGHTGREEN + request.m_requestTarget + " " + LIGHTBLUE + request.m_protokoll);

	if (!validateHttpMethod(methodStr)) {
		setError(WSSC_BAD_REQUEST);
		return;
	}

	if (!validateRequestTarget()) {
		setError(WSSC_BAD_REQUEST);
		return;
	}

	if (!validateProtokoll()) {
		setError(WSSC_HTTP_VERSION_NOT_SUPPORTED);
		return;
	}

	LOG_DEBUG(std::string("validated ~~ RequestLine: ") + LIGHTRED + HTTP::methodToString(request.m_method) + " " + LIGHTGREEN + request.m_URI + " " + LIGHTBLUE + request.m_protokoll);

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
		setError(WSSC_BAD_REQUEST);
		return false;
	}
		
	setError(WSSC_METHOD_NOT_ALLOWED);
	return false;
}

bool	RequestParser::validateRequestTarget(void)
{
	// reject asterisk-form and absolute-form and return early if we don't have a '/'
	if (request.m_requestTarget == "*" ||
		request.m_requestTarget.find("://") != std::string::npos ||
		request.m_requestTarget.find('/') != 0) {
		setError(WSSC_BAD_REQUEST);
		return false;
	}

	if (!validRawCharacters(request.m_requestTarget) ||
		!percentDecoding(request.m_requestTarget, request.m_URI) ||
		!validDecodedCharacters(request.m_URI) ||
	    !isRelativeForm_EnsureLeadingSlash(request.m_URI) ||
	    !removeDotSegments(request.m_URI) ||
		!collapseDuplicateSlashes(request.m_URI)) {
		setError(WSSC_BAD_REQUEST);
		return false;
	}

	const int MAX_URI_LENGTH = 2048;
	if (request.m_URI.empty() ||
		request.m_URI.size() > MAX_URI_LENGTH ||
		request.m_URI.find('\\') != std::string::npos) {
		setError(WSSC_BAD_REQUEST);
		return false;
	}
		
	return true;
}

bool	RequestParser::validateProtokoll(void)
{
	if (request.m_protokoll == "HTTP/1.1")
		return true;

	if (request.m_protokoll == "HTTP/1.0") {
		request.m_protokoll = "HTTP/1.1";
		return true;
	}
	return false;
}

bool	RequestParser::validRawCharacters(const std::string& requestTarget)
{
	for (auto it = requestTarget.cbegin(); it < requestTarget.cend(); ++it) {
		if (*it <= 0x1F || *it == 0x7F || *it == ' ') {
			LOG_DEBUG("failed validRawCharacters(): " + Utils::charToHex(*it));
			return false;
		}
	}
	return true;
}

bool	RequestParser::validDecodedCharacters(const std::string& uri)
{
	for (auto it = uri.cbegin(); it < uri.cend(); ++it) {
		if (*it <= 0x1F || *it == 0x7F || *it == '\\') {
			LOG_DEBUG("failed validDecodedCharacters(): " + Utils::charToHex(*it));
			return false;
		}

		// This is very strict because of filesystem restrictions, but RFC 3986 2.2 allows
		if (*it == '\"' || *it == '<' || *it == '>' ||
			*it == '^' || *it == '`' || *it == '{' || *it == '}' ||
			*it == '|') {
			LOG_DEBUG("failed validDecodedCharacters(): " + Utils::charToHex(*it));
			return false;
		}
	}
	return true;
}

const std::string	RequestParser::truncateQueryAndFragments(const std::string& requestTarget)
{
	size_t pos = requestTarget.find_first_of("?#");
	if (pos != std::string::npos)
		return requestTarget.substr(0, pos);
	return requestTarget;
}

bool	RequestParser::percentDecoding(const std::string& requestTarget, std::string& destURI)
{
	const std::string truncTarget = truncateQueryAndFragments(requestTarget);
	destURI.reserve(truncTarget.size());
	
	for (size_t idx = 0; idx < truncTarget.size(); ++idx) {
		if (truncTarget[idx] == '%') {
			if (idx + 2 < truncTarget.size() &&
			    std::isxdigit(static_cast<unsigned char>(truncTarget[idx + 1])) &&
			    std::isxdigit(static_cast<unsigned char>(truncTarget[idx + 2]))) {
					// Valid percent-encoding: decode and append character
					char hiHex = truncTarget[++idx];
					char loHex = truncTarget[++idx];
					destURI += static_cast<char>((hexToInt(hiHex) << 4) | hexToInt(loHex));
			} else {
				// invalid Hex character, or too short for valid hex form: xFF
				return false;
			}
		} else {
			destURI += truncTarget[idx];
		}	
	}
	return true;
}

int	RequestParser::hexToInt(const char c)
{
	return std::isdigit(c) ? c - '0' : std::tolower(c) - 'a' + 10;
}

bool	RequestParser::isRelativeForm_EnsureLeadingSlash(std::string& uri)
{
	if (Utils::startsWith(uri, "http://") || Utils::startsWith(uri, "https://"))
		return false;

	// make URI absolute per RFC 7230: Section 5.3.1
	if (!uri.empty() && uri[0] != '/')
		uri.insert(0, "/");

	return true;
}

//	RFC:	5.2.4.  Remove Dot Segments
bool	RequestParser::removeDotSegments(std::string& uri)
{
	// ensure we have an absolute path startiung with '/'
	if (uri.empty() || uri[0] != '/')
		return false;
	
	std::string iBuf = uri;
	std::string oBuf;
	oBuf.reserve(uri.size());

	while (!iBuf.empty()) {
		if (Utils::startsWith(iBuf, "../")) {
			iBuf.erase(0, 3);
		} else if (Utils::startsWith(iBuf, "./")) {
			iBuf.erase(0, 2);
		} else if (Utils::startsWith(iBuf, "/./")) {
			iBuf.replace(0, 3, "/");
		} else if (Utils::startsWith(iBuf, "/.") &&
		           (iBuf.size() == 2 || iBuf[2] == '/')) {
			iBuf.replace(0, 2, "/");
		} else if (Utils::startsWith(iBuf,"/../")) {
			iBuf.replace(0, 4, "/");
			popLastSegment(oBuf);
		} else if (Utils::startsWith(iBuf,"/..") &&
		           (iBuf.size() == 3 || iBuf[3] == '/')) {
			iBuf.replace(0, 3, "/");
			popLastSegment(oBuf);
		} else if (iBuf == "." || iBuf == "..") {
			iBuf.clear();
		} else {
			// move first seg from iBuf to oBuf
			size_t pos;
			if (iBuf[0] == '/') {
				if ((pos = iBuf.find("/", 1)) != std::string::npos) {
					oBuf += iBuf.substr(0, pos);
					iBuf.erase(0, pos);
				} else {
					oBuf += iBuf;
					iBuf.clear();
				}
			} else {
				// Unexpected uri segment without leading '/' => _400_BAD_REQUEST
				return false;
			}
		}
	}
	if (oBuf.empty())
		oBuf = "/";
	uri = oBuf;
	return true;
}

void	RequestParser::popLastSegment(std::string& oBuf)
{
	size_t pos = oBuf.rfind("/");
	if (pos != std::string::npos) {
		if (pos == 0)
			oBuf = "/";
		else
			oBuf.erase(pos);
	}
}

bool	RequestParser::collapseDuplicateSlashes(std::string& oBuf)
{
	size_t write = 0;
	bool prevWasSlash = false;

	for (size_t read = 0; read < oBuf.size(); ++read) {
		if (oBuf[read] == '/') {
			if (!prevWasSlash) {
				oBuf[write++] = '/';
				prevWasSlash = true;
			}
		} else {
			oBuf[write++] = oBuf[read];
			prevWasSlash = false;
		}
	}
	oBuf.resize(write);
	return true;
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
