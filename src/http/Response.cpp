#include "Response.hpp"

Response::Response()
	:	statusCode(200),
		m_state(ResponseState::SEND_HEADER),
		m_protocol("HTTP/1.1"),
		m_status_msg(HTTP::getStatusMessage(WSSC_OK)),
		m_bodyType(BodyType::BODY_NONE),
		m_done(false)
{
}

Response::~Response()
{
}

/* ************************************************************************** */
/* ************************************************************************** */

void	Response::setProtocol(const std::string& _protocol)
{
	m_protocol = _protocol;
}

void	Response::setStatusCode(const int& _statusCode)
{
	statusCode = _statusCode;
	m_status_msg = HTTP::getStatusMessage(statusCode);
}

void	Response::addHeader(const std::string& key, const std::string& value)
{
	m_headers[key] = value;
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

std::string	Response::getNextChunk(void)
{
	std::string	buff;

	switch (m_state) {
		case (ResponseState::SEND_HEADER):
			buff = getHeader();
			setState(m_bodyType == BodyType::BODY_NONE ? ResponseState::SEND_COMPLETE : ResponseState::SEND_BODY);
			break ;
		case (ResponseState::SEND_BODY):
			buff = getNextBodyChunk();
			checkBodyState();
			break ;
		case (ResponseState::SEND_COMPLETE):
			m_done = true;
			break ;
		default:
			break ;
	}

	return (buff);
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

bool	Response::complete(void) const
{
	return ( m_done );
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


/* ************************************************************************** */
/* ************************************************************************** */


std::string	Response::getHeader(void) const
{
	std::string	buff;
	buff	+= m_protocol + " "
			+ std::to_string(statusCode) + " "
			+ m_status_msg + "\r\n";
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

void	Response::setState(ResponseState _state)
{
	m_state = _state;
	if (m_state == ResponseState::SEND_COMPLETE || m_state == ResponseState::SEND_ERROR)
		m_done = true;
}
