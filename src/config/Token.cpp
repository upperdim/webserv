/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Token.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 15:46:19 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/10 17:39:57 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Token.hpp"

Token::Token(const TokenType& _type, const std::string& _value, size_t _line)
	:	type(_type),
		keywordType(KeywordType::NONE),
		value(_value),
		line(_line)
{
}

Token::Token(const TokenType& _type, size_t _line)
	:	type(_type),
		keywordType(KeywordType::NONE),
		value(""),
		line(_line)
{
}

Token::~Token()
{
}


/* ************************************************************************** */
/* ************************************************************************** */


std::string	Token::toString(void)	// TODO: delete me
{
	std::string tokenStr = std::string("\033[32m[\033[92m") + std::to_string(line) + "\033[32m] \033[96m";
	tokenStr += tokenTypeToString() + " \033[91m" + tokenKeywordTypeToString();
	tokenStr += " \033[33m\033[93m" + value + "\033[0m";
	return (tokenStr);
}

std::string	Token::getTokenValue(void)
{
	switch (type) {
		case (TokenType::KEYWORD):		return lowerCase(tokenKeywordTypeToString());
		case (TokenType::PARAM):		return value;
		case (TokenType::URI):			return value;
		case (TokenType::NUMBER):		return value;
		case (TokenType::COLON):		return value;
		case (TokenType::SEMICOLON):	return value;
		case (TokenType::OPEN_BRACE):	return value;
		case (TokenType::CLOSE_BRACE):	return value;
		case (TokenType::END_OF_INPUT):	return "end_of_input";
		case (TokenType::INVALID):		return "invalid_token";
		default:						return "unknown_token";
	}
}

std::string	Token::onLine(void)
{
	return (std::string("on line \033[96m") + std::to_string(line));
}


/* ************************************************************************** */
/* ************************************************************************** */


std::string	Token::tokenTypeToString(void)
{
	switch (type) {
		case TokenType::KEYWORD: return "KEYWORD";
		case TokenType::PARAM: return "PARAM";
		case TokenType::URI: return "URI";
		case TokenType::NUMBER: return "NUMBER";
		case TokenType::COLON: return "COLON";
		case TokenType::SEMICOLON: return "SEMICOLON";
		case TokenType::OPEN_BRACE: return "OPEN_BRACE";
		case TokenType::CLOSE_BRACE: return "CLOSE_BRACE";
		case TokenType::END_OF_INPUT: return "END_OF_INPUT";
		case TokenType::INVALID: return "INVALID";
		default: return "UNKNOWN_TOKEN";
	}
}

std::string	Token::tokenKeywordTypeToString(void)
{
	switch (keywordType) {
		case KeywordType::NONE: return "NONE";
		case KeywordType::EVENTS: return "EVENTS";
		case KeywordType::HTTP: return "HTTP";
		case KeywordType::SERVER: return "SERVER";
		case KeywordType::LOCATION: return "LOCATION";
		case KeywordType::LISTEN: return "LISTEN";
		case KeywordType::SERVER_NAME: return "SERVER_NAME";
		case KeywordType::ERROR_PAGE: return "ERROR_PAGE";
		case KeywordType::CLIENT_MAX_BODY_SIZE: return "CLIENT_MAX_BODY_SIZE";
		case KeywordType::ROOT: return "ROOT";
		case KeywordType::INDEX: return "INDEX";
		case KeywordType::ALLOW_METHODS: return "ALLOW_METHODS";
		case KeywordType::RETURN: return "RETURN";
		case KeywordType::AUTOINDEX: return "AUTOINDEX";
		case KeywordType::CGI_EXTENSION: return "CGI_EXTENSION";
		case KeywordType::ALLOW_UPLOAD: return "ALLOW_UPLOAD";
		case KeywordType::UPLOAD_STORE: return "UPLOAD_STORE";
		default: return "UNKNOWN_KEYWORD_TYPE";
	}
}

std::string Token::lowerCase(const std::string& str)
{
	std::string	lower;
	for (std::string::const_iterator it = str.cbegin(); it < str.cend(); ++it)
		lower += std::tolower(*it);
	return (lower);	
}
