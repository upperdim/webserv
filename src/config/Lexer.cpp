/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 11:38:25 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/09 11:32:37 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Lexer.hpp"

Lexer::Lexer(const std::string& _input)
	:	m_input(_input),
		m_pos(0),
		m_line(1)
{
}

Lexer::Lexer(const Lexer& other)
	:	m_input(other.m_input),
		m_pos(0),
		m_line(other.m_line)
{
}

Lexer::~Lexer()
{
}


/* ************************************************************************** */
/* ************************************************************************** */


Token	Lexer::nextToken(void)
{
	precededByComment = false;
	skipWhitespaces();

	if (m_pos >= m_input.length())
		return ( Token(TokenType::END_OF_INPUT, "") );

	if (m_input[m_pos] == '#')
	{
		precededByComment = true;
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
			return ( Token(TokenType::OPEN_BRACE, m_line) );
		case '}':
			++m_pos;
			return ( Token(TokenType::CLOSE_BRACE, m_line) );
		case ';':
			++m_pos;
			return ( Token(TokenType::SEMICOLON, m_line) );
		case ':':
			++m_pos;
			return ( Token(TokenType::COLON, m_line) );
		case '/':
		{
			// URI
			std::string uri;
			std::string validCharacters("-_.~/?#[]=&%");
			while ( m_pos < m_input.length() && (std::isalnum(m_input[m_pos]) || validCharacters.find(m_input[m_pos]) != std::string::npos) )
				uri += m_input[m_pos++];
			return ( Token(TokenType::URI, uri) );
		}
		default:
			if (std::isalpha(m_input[m_pos]))
			{
				// KEYWORD or PARAM/STRING
				std::string word;
				std::string validCharacters(".");
				while ( m_pos < m_input.length() && (std::isalpha(m_input[m_pos]) || validCharacters.find(m_input[m_pos]) != std::string::npos) )
					word += m_input[m_pos++];

				// check for keyword
				if (word == "events")
					return ( Token(TokenType::EVENTS, m_line) );
				if (word == "http")
					return ( Token(TokenType::HTTP, m_line) );
				if (word == "server")
					return ( Token(TokenType::SERVER, m_line) );
				if (word == "location")
					return ( Token(TokenType::LOCATION, m_line) );
				if (word == "listen")
					return ( Token(TokenType::LISTEN, m_line) );

				return ( Token(TokenType::STRING, word, m_line) );
			}
			else if ( std::isdigit(m_input[m_pos]) )
			{
				// NUMBER or IP
				std::string num;
				size_t      epos = isIPAddress();
				if (epos)
				{
					num = m_input.substr(m_pos, epos - m_pos);
					m_pos = epos;
					return ( Token(TokenType::IP, num) );
				}
				else
				{
					while ( m_pos < m_input.length() && std::isdigit(m_input[m_pos]) )
						num += m_input[m_pos++];
				}

				return ( Token(TokenType::NUMBER, num) );
			}
	}

	m_pos = m_input.length();	// TODO: delete me
	return ( Token(TokenType::ERROR, "Unexpected character") );
}


/* ************************************************************************** */
/* ************************************************************************** */


void	Lexer::skipWhitespaces(void)
{
	while (m_pos < m_input.length() && std::isspace(m_input[m_pos]))
	{
		if (m_input[m_pos] == '\n')
			++m_line;
		m_pos++;
	}
}

size_t	Lexer::isIPAddress() const
{
	size_t	cpos = m_pos;
	int		octetCount = 0;
	int		currentOctet;

	if (cpos + 7 > m_input.length())
		return (0);

	while (cpos < m_input.length() && octetCount < 4)
	{
		currentOctet = 0;
		int digits = 0;
		
		while (cpos < m_input.length() && std::isdigit(m_input[cpos]) && digits < 3)
		{
			currentOctet = currentOctet * 10 + (m_input[cpos++] - '0');
			++digits;
		}

		if (currentOctet > 255)
			return (0);
		
		if (octetCount < 3)
		{
			if (cpos >= m_input.length() || m_input[cpos] != '.')
				return (0);
			++cpos;
		}

		++octetCount;
	}

	if ( octetCount == 4 && (cpos == m_input.length() || std::isdigit(m_input[cpos]) == false) )
		return (cpos);
	return (0);
}
