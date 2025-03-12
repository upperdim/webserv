/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 11:29:39 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/12 12:02:19 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response()
	:	m_protokoll("HTTP/1.1"),
		m_status_code(200),
		m_status_msg(HTTP::getStatusMessage(WSSC_OK))
{
}

Response::Response(const Response& other)
	:	m_protokoll(other.m_protokoll),
		m_status_code(other.m_status_code),
		m_status_msg(other.m_status_msg),
		m_headers(other.m_headers),
		m_body(other.m_body)
{
}

Response::~Response()
{
}


/* ************************************************************************** */
/* ************************************************************************** */

Response&	Response::operator=(const Response& rhs)
{
	if (this != &rhs)
	{
		m_protokoll = rhs.m_protokoll;
		m_status_code = rhs.m_status_code;
		m_status_msg = rhs.m_status_msg;
		m_headers = rhs.m_headers;
		m_body = rhs.m_body;
	}
	return (*this);
}


/* ************************************************************************** */
/* ************************************************************************** */


void	Response::setProtokoll(const std::string& _protokoll)
{
	m_protokoll = _protokoll;
}

void	Response::setStatus(const int& _status_code, const std::string& _status_msg)
{
	m_status_code = _status_code;
	m_status_msg = _status_msg;
}

void	Response::addHeader(const std::string& key, const std::string& value)
{
	m_headers[key] = value;
}

void	Response::setBody(const std::string& _body)
{
	m_body = _body;
	m_headers["Content-Length"] = std::to_string(m_body.size());
}

std::string	Response::to_string(void) const
{
	std::string	response;
	response	+= m_protokoll + " "
				+ std::to_string(m_status_code) + " "
				+ m_status_msg + "\r\n";
	for (const auto& header : m_headers)
		response += header.first + ": " + header.second + "\r\n";
	response += "\r\n";
	response += m_body;
	return (response);
}
