/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 11:29:39 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/19 14:54:47 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response()
	:	m_isComplete(false),
		m_state(ResponseState::SEND_HEADER),
		m_protokoll("HTTP/1.1"),
		m_status_code(200),
		m_status_msg(HTTP::getStatusMessage(WSSC_OK)),
		m_body_type(BodyType::BODY_NONE),
		m_done(false)
{
}

// move constructor
Response::Response(Response&& other)
	:	m_state(other.m_state),
		m_protokoll(other.m_protokoll),
		m_status_code(other.m_status_code),
		m_status_msg(other.m_status_msg),
		m_headers(std::move(other.m_headers)),
		m_body(other.m_body),
		m_body_type(other.m_body_type),
		m_done(other.m_done)
{
	m_file_buffer_reader = std::move(other.m_file_buffer_reader);
}

Response::~Response()
{
}


/* ************************************************************************** */
/* ************************************************************************** */

// Response&	Response::operator=(const Response& rhs)
// {
// 	if (this != &rhs)
// 	{
// 		m_state = rhs.m_state;
// 		m_protokoll = rhs.m_protokoll;
// 		m_status_code = rhs.m_status_code;
// 		m_status_msg = rhs.m_status_msg;
// 		m_headers = rhs.m_headers;
// 		m_body = rhs.m_body;
// 		m_file_buffer_reader = rhs.m_file_buffer_reader;
// 		m_body_type = rhs.m_body_type;
// 		m_done = rhs.m_done;		
// 	}
// 	return (*this);
// }

Response&	Response::operator=(Response&& rhs)
{
	if (this != &rhs)
	{
		m_state = rhs.m_state;
		m_protokoll = std::move(rhs.m_protokoll);
		m_status_code = std::move(rhs.m_status_code);
		m_status_msg = rhs.m_status_msg;
		m_headers = std::move(rhs.m_headers);
		m_body = std::move(rhs.m_body);
		m_file_buffer_reader = std::move(rhs.m_file_buffer_reader);
		m_body_type = rhs.m_body_type;
		m_done = rhs.m_done;
	}
	return (*this);
}

/* ************************************************************************** */
/* ************************************************************************** */


void	Response::setProtokoll(const std::string& _protokoll)
{
	m_protokoll = _protokoll;
}

void	Response::setStatus(const int& _status_code)
{
	m_status_code = _status_code;
	m_status_msg = HTTP::getStatusMessage(_status_code);
}

void	Response::addHeader(const std::string& key, const std::string& value)
{
	m_headers[key] = value;
}

void	Response::setBodyString(const std::string& _body)
{
	m_body_type = BodyType::BODY_STRING;
	m_body = _body;
	m_headers["Content-Length"] = std::to_string(m_body.size());
}

void	Response::setBodyFileBufferReader(std::string path)
{
	m_body_type = BodyType::BODY_FILE_BUFFER;
	m_file_buffer_reader = FileBufferReader(path, RESPONSE_BUFFER_SIZE);
	m_headers["Content-Length"] = std::to_string(m_file_buffer_reader.getSize());
}

std::string	Response::getNextChunk(void)
{
	std::string	buff;

	switch (m_state)
	{
		case (ResponseState::SEND_HEADER):
			buff = getHeader();
			progressState();
			break ;
		case (ResponseState::SEND_BODY):
			buff = getNextBodyChunk();
			progressState();
			break ;
		case (ResponseState::SEND_COMPLETE):
			m_done = true;
			break ;
		default:
			break ;
	}
	return (buff);
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
	buff	+= m_protokoll + " "
			+ std::to_string(m_status_code) + " "
			+ m_status_msg + "\r\n";
	for (const auto& header : m_headers)
		buff += header.first + ": " + header.second + "\r\n";
	buff += "\r\n";
	return (buff);
}

std::string	Response::getNextBodyChunk(void)
{
	std::stringstream	ss;

	switch (m_body_type)
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

void	Response::progressState(void)
{
	switch (m_state)
	{
		case (ResponseState::SEND_HEADER):
			setState(ResponseState::SEND_BODY);
			if (m_body_type == BodyType::BODY_NONE)
				setState(ResponseState::SEND_COMPLETE);
			break ;
		case (ResponseState::SEND_BODY):
			if (m_body_type == BodyType::BODY_STRING)
			{
				if (m_body.empty() || m_body.length() == 0)
					setState(ResponseState::SEND_COMPLETE);
			}
			else if (m_body_type == BodyType::BODY_FILE_BUFFER)
			{
				switch (m_file_buffer_reader.getState())
				{
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
			break ;
		default:
			break ;
	}
}

void	Response::setState(ResponseState _state)
{
	m_state = _state;
	if (m_state == ResponseState::SEND_COMPLETE || m_state == ResponseState::SEND_ERROR)
		m_done = true;
}
