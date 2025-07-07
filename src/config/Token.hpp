
#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "colors.hpp"

enum class TokenType
{
	KEYWORD,		//	server, location, root, etc.
	PARAM,			//	VALUES, DomainName, IP
	PATH,			//	PATH
	URL,			//	Absolute-URL, staring with: http:// https://
	NUMBER,			//	80, 443, 1024, etc.

	COLON,			//	:
	SEMICOLON,		//	;
	OPEN_BRACE,		//	{
	CLOSE_BRACE,	//	}

	END_OF_INPUT,
	INVALID
};

enum class KeywordType {
	NONE,
	EVENTS,
	HTTP,
	SERVER,
	LOCATION,
	LISTEN,
	SERVER_NAME,
	ERROR_PAGE,
	CLIENT_MAX_BODY_SIZE,
	ROOT,
	INDEX,
	ALLOW_METHODS,
	RETURN,
	AUTOINDEX,
	CGI_EXTENSION,
	ALLOW_UPLOAD,
	UPLOAD_STORE	
};

class Token
{
public:
	Token(const TokenType& _type, const KeywordType& _keywordType, const std::string& _value, size_t _line);
	Token(const TokenType& _type, const std::string& _value, size_t _line = 0);
	Token(const TokenType& _type, size_t _line);
	Token(const Token& other);
	~Token();

	TokenType	type;
	KeywordType	keywordType;
	std::string	value;
	size_t		line;

	std::string	toString(void) const;
	std::string	getTokenValue(void) const;
	std::string	inLine(void) const;

private:
	std::string	tokenTypeToString(void) const;
	std::string	tokenKeywordTypeToString(void) const;

	static std::string	lowerCase(const std::string& str);
	size_t				countDigits(size_t nbr);

	static size_t		m_maxLineDigits;

};

std::ostream&  operator << (std::ostream& os, const Token& token);

#endif
