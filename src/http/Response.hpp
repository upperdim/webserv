#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <fstream>
#include <utility>
#include "Request.hpp"
#include "FileBufferReader.hpp"
#include "HTTP.hpp"

// A good default RESPONSE_BUFFER_SIZE seams to be 64kb - 128kb
#define RESPONSE_BUFFER_SIZE (64 * 1024)

typedef std::unordered_map<std::string, std::string> HeaderMap;

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
	void				setAsCgiResponse(void);
	std::string			getNextChunk(void);

	bool				complete(void) const;
	bool				error(void) const;

	// for debugging
	std::string			getResponseStateString();

private:
	enum class ResponseState {
		SEND_HEADER,
		SEND_BODY,
		SEND_COMPLETE,
		SEND_ERROR,
		SEND_CGI
	};

	enum class BodyType {
		BODY_NONE,
		BODY_STRING,
		BODY_FILE_BUFFER,
	};

	ResponseState		m_state;

	std::string			m_protocol;
	int					m_status_code;
	std::string			m_status_msg;

	HeaderMap			m_headers;
	
	std::string			m_body;
	FileBufferReader	m_file_buffer_reader;
	BodyType			m_bodyType;
	bool				m_done;


	Response(const Response& other); // copy constructor =delete

	std::string			getResponseLine(void) const;
	std::string			getHeaders(void) const;
	std::string			getNextBodyChunk(void);
	void				checkBodyState();
	void				setState(ResponseState _state);
};

#endif
