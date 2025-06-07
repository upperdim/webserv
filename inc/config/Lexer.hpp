/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tunsal <tunsal@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 19:11:45 by tunsal            #+#    #+#             */
/*   Updated: 2025/06/06 19:11:45 by tunsal           ###   ########.fr       */
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

	bool	precededByComment = false;
	
	Token	nextToken();

private:
	Lexer&	operator=(const Lexer& rhs);

	void	skipWhitespaces();

	std::string	m_input;
	size_t		m_pos;
};

#endif
