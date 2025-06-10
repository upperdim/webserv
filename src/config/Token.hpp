/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Token.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 15:35:20 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/10 16:55:21 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

enum class TokenType
{
	KEYWORD,		//	server, location, root, etc.
	PARAM,			//	VALUES, TLD, IP
	URI,			//	URI
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
	Token(const TokenType& _type, const std::string& _value, size_t _line = 0);
	Token(const TokenType& _type, size_t _line);
	~Token();

	TokenType	type;
	KeywordType	keywordType;
	std::string	value;
	size_t		line;

	std::string	toString(void);
	std::string	getTokenValue(void);
	std::string	onLine(void);

private:
	std::string	tokenTypeToString(void);
	std::string	tokenKeywordTypeToString(void);

	static std::string lowerCase(const std::string& str);
};

#endif
