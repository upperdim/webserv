/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Token.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 15:46:19 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/04 18:53:44 by nmihaile         ###   ########.fr       */
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
	switch (type)
	{
		case (TokenType::END_OF_INPUT): return ("TOKEN::END_OF_INPUT");
		case (TokenType::STRING):       return ("TOKEN::STRING");
		case (TokenType::NUMBER):       return ("TOKEN::NUMBER");
		case (TokenType::OPEN_BRACE):   return ("TOKEN::OPEN_BRACE");
		case (TokenType::CLOSE_BRACE):  return ("TOKEN::CLOSE_BRACE");
		case (TokenType::SEMICOLON):     return ("TOKEN::SEMICOLON");
	}
	return ("TOKEN::NO_TOKEN");
}
