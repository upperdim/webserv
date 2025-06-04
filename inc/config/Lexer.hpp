/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 11:38:27 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/04 18:54:54 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <cctype>
#include "Token.hpp"
#include "Log.hpp"

class Lexer
{
public:
	Lexer(const std::string& _input);
	Lexer(const Lexer& other);
	~Lexer();

	Token	nextToken();

private:
	Lexer&	operator=(const Lexer& rhs);

	void	skipWhitespaces();

	std::string	m_input;
	size_t		m_pos;
};

#endif
