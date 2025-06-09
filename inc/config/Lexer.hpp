/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 19:11:45 by tunsal            #+#    #+#             */
/*   Updated: 2025/06/09 11:03:32 by nmihaile         ###   ########.fr       */
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
	
	Token	nextToken(void);

private:
	Lexer&	operator=(const Lexer& rhs);

	void	skipWhitespaces(void);
	size_t	isIPAddress(void) const;

	std::string	m_input;
	size_t		m_pos;
	size_t		m_line;
};

#endif
