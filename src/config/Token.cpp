/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Token.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 15:46:19 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/06 16:29:51 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Token.hpp"

Token::Token(const TokenType& _type, const std::string _value)
	:	type(_type),
		value(_value)
{
}

Token::~Token()
{
}


/* ************************************************************************** */
/* ************************************************************************** */


std::string	Token::toString(void)
{
	std::string tokenStr = std::string("[") + tokenTypeToString() + "]";

	if (type == TokenType::HTTP)
		tokenStr += " \033[0mHTTP";
	else if (type == TokenType::SERVER)
		tokenStr += " \033[0mSERVER";
	else if (type == TokenType::LOCATION)
		tokenStr += " \033[0mLOCATION";
	else if (type == TokenType::LISTEN)
		tokenStr += " \033[0mLISTEN";
	else if (type == TokenType::OPEN_BRACE)
		tokenStr += " \033[93m{";
	else if (type == TokenType::CLOSE_BRACE)
		tokenStr += " \033[93m}";
	else if (type == TokenType::SEMICOLON)
		tokenStr += " \033[91m;";
	else if (type == TokenType::URI || type == TokenType::NUMBER || type == TokenType::STRING)
		tokenStr += " \033[96m" + value;

	return (tokenStr);
}

std::string	Token::getTokenValue(void)
{
	switch (type)
	{
		case (TokenType::END_OF_INPUT): return ("end_of_input");
		case (TokenType::EVENTS):       return ("events");
		case (TokenType::HTTP):         return ("http");
		case (TokenType::SERVER):       return ("server");
		case (TokenType::LOCATION):     return ("location");
		case (TokenType::LISTEN):       return ("listen");
		case (TokenType::OPEN_BRACE):   return ("{");
		case (TokenType::CLOSE_BRACE):  return ("}");
		case (TokenType::SEMICOLON):    return (";");
		case (TokenType::URI):          return (value);
		case (TokenType::NUMBER):       return (value);
		case (TokenType::STRING):       return (value);
		case (TokenType::ERROR):        return ("ERROR");
	}
	return ("INVALID_TOKEN");
}


/* ************************************************************************** */
/* ************************************************************************** */


std::string	Token::tokenTypeToString(void)
{
	switch (type)
	{
		case (TokenType::END_OF_INPUT): return ("TOKEN::END_OF_INPUT");
		case (TokenType::EVENTS):       return ("TOKEN::EVENTS");
		case (TokenType::HTTP):         return ("TOKEN::HTTP");
		case (TokenType::SERVER):       return ("TOKEN::SERVER");
		case (TokenType::LOCATION):     return ("TOKEN::LOCATION");
		case (TokenType::LISTEN):       return ("TOKEN::LISTEN");
		case (TokenType::OPEN_BRACE):   return ("TOKEN::OPEN_BRACE");
		case (TokenType::CLOSE_BRACE):  return ("TOKEN::CLOSE_BRACE");
		case (TokenType::SEMICOLON):    return ("TOKEN::SEMICOLON");
		case (TokenType::URI):          return ("TOKEN::URI");
		case (TokenType::NUMBER):       return ("TOKEN::NUMBER");
		case (TokenType::STRING):       return ("TOKEN::STRING");
		case (TokenType::ERROR):        return ("TOKEN::ERROR");
	}
	return ("TOKEN::INVALID_TOKEN");
}
