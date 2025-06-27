/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 11:29:40 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/19 14:54:55 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <fstream>
#include <utility>
#include "webserv.hpp"
#include "Request.hpp"
#include "FileBufferReader.hpp"
#include "HTTP.hpp"

class Response
{
public:
	Response();
	Response(Response&& other);			// move constructor
	~Response();

	void				setProtokoll(const std::string& _protokoll);
	void				setStatus(const int& _status_code);
	void				addHeader(const std::string& key, const std::string& value);
	void				setBodyString(const std::string& _body);
	void				setBodyFileBufferReader(std::string path);
	std::string			getNextChunk(void);

	bool				complete(void) const;
	bool				error(void) const;
	
	// for debugging
	std::string			getResponseStateString();

	bool				isComplete;

private:
	enum class ResponseState {
		SEND_HEADER,
		SEND_BODY,
		SEND_COMPLETE,
		SEND_ERROR
	};

	enum class BodyType {
		BODY_NONE,
		BODY_STRING,
		BODY_FILE_BUFFER,
	};

	ResponseState		m_state;
	std::string			m_protokoll;
	int					m_status_code;
	std::string			m_status_msg;
	HeaderMap			m_headers;
	std::string			m_body;
	FileBufferReader	m_file_buffer_reader;
	BodyType			m_body_type;
	bool				m_done;

	Response(const Response& other); // copy constructor =delete

	std::string			getHeader(void) const;
	std::string			getNextBodyChunk(void);
	void				progressState(void);
	void				setState(ResponseState _state);
};

#endif
