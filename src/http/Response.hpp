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
	~Response();

	void				setProtocol(const std::string& _protocol);
	void				setStatusCode(const int& _statusCode);
	void				addHeader(const std::string& key, const std::string& value);
	void				setBodyString(const std::string& _body);
	void				setBodyFileBufferReader(std::string path);
	std::string			getNextChunk(void);

	bool				complete(void) const;
	bool				error(void) const;
	
	int					statusCode;

	// for debugging
	std::string			getResponseStateString();

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
	std::string			m_protocol;
	std::string			m_status_msg;
	HeaderMap			m_headers;
	std::string			m_body;
	FileBufferReader	m_file_buffer_reader;
	BodyType			m_bodyType;
	bool				m_done;


	Response(const Response& other); // copy constructor =delete

	std::string			getHeader(void) const;
	std::string			getNextBodyChunk(void);
	void				checkBodyState();
	void				setState(ResponseState _state);
};

#endif
