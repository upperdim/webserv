/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 11:38:25 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/06 17:05:36 by nmihaile         ###   ########.fr       */
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
		skipWhitespaces();
		if (m_pos >= m_input.length())
			return ( Token(TokenType::END_OF_INPUT, "") );
	}

	switch (m_input[m_pos])
	{
		case '{':
			++m_pos;
			return ( Token(TokenType::OPEN_BRACE) );
		case '}':
			++m_pos;
			return ( Token(TokenType::CLOSE_BRACE) );
		case ';':
			++m_pos;
			return ( Token(TokenType::SEMICOLON) );
		case '/':
		{
			// URI
			std::string uri;
			while ( m_pos < m_input.length() && (std::isalnum(m_input[m_pos]) || std::string("-_.~/?#[]=&%").find(m_input[m_pos]) != std::string::npos) )
				uri += m_input[m_pos++];
			return ( Token(TokenType::URI, uri) );
		}
		default:
			if (std::isalpha(m_input[m_pos]))
			{
				// KEYWORD or STRING
				std::string word;
				while ( m_pos < m_input.length() && std::isalpha(m_input[m_pos]) )
					word += m_input[m_pos++];

				// check for keyword
				if (word == "events")
					return ( Token(TokenType::EVENTS) );
				if (word == "http")
					return ( Token(TokenType::HTTP) );
				if (word == "server")
					return ( Token(TokenType::SERVER) );
				if (word == "location")
					return ( Token(TokenType::LOCATION) );

				return ( Token(TokenType::STRING, word) );
			}
			else if ( std::isdigit(m_input[m_pos]) )
			{
				// NUMBER
				std::string num;
				while ( m_pos < m_input.length() && (std::isdigit(m_input[m_pos]) || m_input[m_pos] == '.') )
					num += m_input[m_pos++];

				return ( Token(TokenType::NUMBER, num) );
			}
	}

	m_pos = m_input.length();	// TODO: delete me
	return ( Token(TokenType::ERROR, "Unexpected character") );
}


/* ************************************************************************** */
/* ************************************************************************** */


void	Lexer::skipWhitespaces()
{
	while (m_pos < m_input.length() && std::isspace(m_input[m_pos]))
		m_pos++;
}
