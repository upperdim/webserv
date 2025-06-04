/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Token.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 15:46:19 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/04 19:23:33 by nmihaile         ###   ########.fr       */
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
	std::string tokenStr = std::string("[");
	tokenStr += tokenTypeToString() + "] " + value;

	return (tokenStr);
}


/* ************************************************************************** */
/* ************************************************************************** */


std::string	Token::tokenTypeToString(void)
{
	switch (type)
	{
		case (TokenType::END_OF_INPUT): return ("TOKEN::END_OF_INPUT");
		case (TokenType::KEYWORD):      return ("TOKEN::KEYWORD");
		case (TokenType::STRING):       return ("TOKEN::STRING");
		case (TokenType::NUMBER):       return ("TOKEN::NUMBER");
		case (TokenType::OPEN_BRACE):   return ("TOKEN::OPEN_BRACE");
		case (TokenType::CLOSE_BRACE):  return ("TOKEN::CLOSE_BRACE");
		case (TokenType::SEMICOLON):    return ("TOKEN::SEMICOLON");
		case (TokenType::ERROR):        return ("TOKEN::ERROR");
	}
	return ("TOKEN::NO_TOKEN");
}
