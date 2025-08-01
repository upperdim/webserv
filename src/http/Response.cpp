#include <random>
#include "Response.hpp"

Response::Response()
	:	m_state(ResponseState::SEND_HEADER),
		m_protocol("HTTP/1.1"),
		m_status_code(200),
		m_status_msg(HTTP::getStatusMessage(WSSC_OK)),
		m_bodyType(BodyType::BODY_NONE),
		m_done(false)
{
}

Response::~Response()
{
}

void	Response::setProtocol(const std::string& _protocol)
{
	m_protocol = _protocol;
}

void	Response::setStatusCode(const int& _statusCode)
{
	m_status_code = _statusCode;
	m_status_msg = HTTP::getStatusMessage(m_status_code);
}

void	Response::addHeader(const std::string& key, const std::string& value)
{
	m_headers[key] = value;
}

void	Response::createSessionCookie()
{
	static const char charset[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	std::string sessionToken;
	sessionToken.reserve(SESSION_TOKEN_LEN);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);

	for (size_t i = 0; i < SESSION_TOKEN_LEN; ++i)
		sessionToken += charset[dist(gen)];

	std::ostringstream cookieVal;
	cookieVal << "sessionId=" << sessionToken
			<< "; HttpOnly"
			<< "; SameSite=Strict";
	
	addHeader("Set-Cookie", cookieVal.str());
}

void	Response::setBodyString(const std::string& _body)
{
	m_bodyType = BodyType::BODY_STRING;
	m_body = _body;
	m_headers["Content-Length"] = std::to_string(m_body.size());
}

void	Response::setBodyFileBufferReader(std::string path)
{
	m_bodyType = BodyType::BODY_FILE_BUFFER;
	m_file_buffer_reader = FileBufferReader(path, RESPONSE_BUFFER_SIZE);
	m_headers["Content-Length"] = std::to_string(m_file_buffer_reader.getSize());
}

void	Response::setAsCgiResponse(void)
{
	setState(ResponseState::SEND_CGI);
}

std::string	Response::getNextChunk(void)
{
	std::string	buff;

	switch (m_state) {
		case (ResponseState::SEND_HEADER):
			buff = getHeaders();
			setState(m_bodyType == BodyType::BODY_NONE ? ResponseState::SEND_COMPLETE : ResponseState::SEND_BODY);
			break ;
		case (ResponseState::SEND_BODY):
			buff = getNextBodyChunk();
			checkBodyState();
			break ;
		case (ResponseState::SEND_COMPLETE):
			setComplete();
			break ;
		case (ResponseState::SEND_CGI):
			buff = getResponseLine();
			if (m_bodyType != BodyType::BODY_FILE_BUFFER) {
				LOGT(Log::ERROR, "CGI response body type is different than BODY_FILE_BUFFER");
			}
			setState(ResponseState::SEND_BODY);
			break;
		default:
			break ;
	}

	return (buff);
}

bool	Response::complete(void) const
{
	return ( m_done );
}

void	Response::setComplete(void)
{
	m_done = true;
}

bool	Response::error(void) const
{
	return (m_state == ResponseState::SEND_ERROR);
}

std::string			Response::getResponseStateString()
{
	switch (m_state)
	{
		case (ResponseState::SEND_HEADER):
			return ("SEND_HEADER");
		case (ResponseState::SEND_BODY):
			return ("SEND_BODY");
		case (ResponseState::SEND_COMPLETE):
			return ("SEND_COMPLETE");
		case (ResponseState::SEND_ERROR):
			return ("SEND_ERROR");
		default:
			break ;
	}
	return ("CRITICAL_ERROR_::_RESPONSE_STATE_NOT_SET_TO_VALID_STATE");
}

std::string Response::getResponseLine(void) const
{
	std::string	buff;
	buff	+= m_protocol + " "
			+ std::to_string(m_status_code) + " "
			+ m_status_msg + "\r\n";
	return (buff);
}

std::string	Response::getHeaders(void) const
{
	std::string buff = getResponseLine();
	for (const auto& header : m_headers)
		buff += header.first + ": " + header.second + "\r\n";
	buff += "\r\n";
	return (buff);
}

std::string	Response::getNextBodyChunk(void)
{
	std::stringstream	ss;

	switch (m_bodyType)
	{
		case (BodyType::BODY_STRING):
			ss << m_body.substr(0, RESPONSE_BUFFER_SIZE);
			m_body.erase(0, RESPONSE_BUFFER_SIZE);
			break ;
		case (BodyType::BODY_FILE_BUFFER):
			ss << m_file_buffer_reader.getNextChunk();
			if (m_file_buffer_reader.error())
				setState(ResponseState::SEND_ERROR);
			break ;
		default:
			break ;
	}

	return ( ss.str() );
}

void	Response::checkBodyState()
{
	if (m_bodyType == BodyType::BODY_STRING) {
		if (m_body.empty()) {
			setState(ResponseState::SEND_COMPLETE);
		}
	} else if (m_bodyType == BodyType::BODY_FILE_BUFFER) {
		switch (m_file_buffer_reader.getState()) {
			case (FileBuffer::State::COMPLETE):
				setState(ResponseState::SEND_COMPLETE);
				break ;
			case (FileBuffer::State::ERROR):
				setState(ResponseState::SEND_ERROR);
				break ;
			default:
				break ;
		}
	}
}

void	Response::setState(ResponseState _state)
{
	m_state = _state;
	if (m_state == ResponseState::SEND_COMPLETE || m_state == ResponseState::SEND_ERROR)
		setComplete();
}
