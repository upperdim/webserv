/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Token.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 15:35:20 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/04 19:34:27 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

enum class TokenType
{
	END_OF_INPUT,
	KEYWORD,
	STRING,
	NUMBER,
	OPEN_BRACE,
	CLOSE_BRACE,
	SEMICOLON,
	ERROR
};

class Token
{
public:
	Token(const TokenType& _type, const std::string _value);
	~Token();

	TokenType	type;
	std::string	value;

	std::string	toString(void);

private:
	std::string	tokenTypeToString(void);
};

#endif
