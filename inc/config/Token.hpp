/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Token.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 15:35:20 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/04 17:25:43 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

enum class TokenType
{
	END_OF_INPUT,
	STRING,
	NUMBER,
	OPEN_BRACE,
	CLOSE_BRACE,
	SEMICOLON
};

class Token
{
public:
	Token(const TokenType& _type, const std::string _value);
	~Token();

	TokenType	type;
	std::string	value;
	
private:
};

#endif
