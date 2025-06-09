/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Token.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 15:35:20 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/09 11:17:52 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

enum class TokenType
{
	END_OF_INPUT,
	EVENTS,
	HTTP,
	SERVER,
	LOCATION,
	LISTEN,
	OPEN_BRACE,
	CLOSE_BRACE,
	SEMICOLON,
	COLON,
	URI,
	NUMBER,
	IP,
	STRING,
	ERROR
};

class Token
{
public:
	Token(const TokenType& _type, const std::string _value, size_t _line = 0);
	Token(const TokenType& _type, size_t _line);
	~Token();

	TokenType	type;
	std::string	value;
	size_t		line;

	std::string	toString(void);
	std::string	getTokenValue(void);
	std::string	onLine(void);

private:
	std::string	tokenTypeToString(void);
};

#endif
