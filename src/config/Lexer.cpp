/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 11:38:25 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/04 19:18:20 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Lexer.hpp"

Lexer::Lexer(const std::string& _input)
	:	m_input(_input),
		m_pos(0)
{
}

Lexer::Lexer(const Lexer& other)
	:	m_input(other.m_input),
		m_pos(0)
{
}

Lexer::~Lexer()
{
}


/* ************************************************************************** */
/* ************************************************************************** */


Token	Lexer::nextToken()
{
	skipWhitespaces();

	if (m_pos >= m_input.length())
		return ( Token(TokenType::END_OF_INPUT, "") );

	if (m_input[m_pos] == '#')
	{
		size_t next_pos = m_input.find_first_of("\n", m_pos);
		if (next_pos == std::string::npos)
		{
			m_pos = m_input.length();
			return ( Token(TokenType::END_OF_INPUT, "") );
		}
		m_pos = next_pos;
	}

	switch (m_input[m_pos])
	{
		case '{':
			++m_pos;
			return (Token(TokenType::OPEN_BRACE, "{"));
		case '}':
			++m_pos;
			return (Token(TokenType::CLOSE_BRACE, "}"));
		case ';':
			++m_pos;
			return (Token(TokenType::SEMICOLON, ";"));
	}

	m_pos = m_input.length();	// TODO: delete me
	return (Token(TokenType::ERROR, "Unexpected character"));
}


/* ************************************************************************** */
/* ************************************************************************** */


void	Lexer::skipWhitespaces()
{
	while (m_pos < m_input.length() && std::isspace(m_input[m_pos]))
		m_pos++;
}
