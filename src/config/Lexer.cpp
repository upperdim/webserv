/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 11:38:25 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/10 17:46:43 by nmihaile         ###   ########.fr       */
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
		return Token(TokenType::END_OF_INPUT, m_line);

	char c = peek();

	if (c == '#') {
		skipComment();
		if (m_pos >= m_input.length())
			return Token(TokenType::END_OF_INPUT, m_line);
	}

	switch (c) {
		case ':':
			advance();
			return Token(TokenType::COLON, ":", m_line);
		case ';':
			advance();
			return Token(TokenType::SEMICOLON, ";", m_line);
		case '{':
			advance();
			return Token(TokenType::OPEN_BRACE, "{", m_line);
		case '}':
			advance();
			return Token(TokenType::CLOSE_BRACE, "}", m_line);
		case '/':
		{
			// URI
			std::string uri(1, c);
			std::string validCharacters("-_.~/?#[]=&%");
			while ( std::isalnum(m_input[m_pos]) || validCharacters.find(m_input[m_pos]) != std::string::npos )
				uri += advance();
			return ( Token(TokenType::URI, uri) );
		}
		default:
			if (std::isalpha(c))
			{
				// KEYWORD or PARAM/STRING
				std::string word(1, c);
				std::string validCharacters(".");
				while ( std::isalpha(m_input[m_pos]) || validCharacters.find(m_input[m_pos]) != std::string::npos )
					word += advance();

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
			else if ( std::isdigit(c) )
			{
				// NUMBER or IP
				std::string num(1, c);
				size_t      epos = isIPAddress(m_pos - 1);
				if (epos)
				{
					num = m_input.substr(m_pos - 1, epos - m_pos);
					m_pos = epos;
					return ( Token(TokenType::IP, num) );
				}

				while ( std::isdigit(m_input[m_pos]) )
					num += advance();
				return ( Token(TokenType::NUMBER, num) );
			}
	}

	m_pos = m_input.length();	// TODO: delete me
	return ( Token(TokenType::ERROR, "Unexpected character") );
}


/* ************************************************************************** */
/* ************************************************************************** */


void	Lexer::markStart(void)
{
	m_startPos = m_pos;
}

char	Lexer::peek(size_t offset = 0) const
{
	size_t idx = m_pos + offset;
	return  idx < m_input.length() ? m_input[idx] : '\0';
}

char	Lexer::advance(void)
{
	if (m_pos < m_input.length()) {
		char c = m_input[m_pos++];
		if (c == '\n')
			++m_line;
		return c;
	}
	return '\0';
}

void	Lexer::skipWhitespaces(void)
{
	while (m_pos < m_input.length() && std::isspace(peek()))
		advance();
}

void	Lexer::skipComment(void)
{
	precededByComment = true;
	advance();	//	consume '#'
	while (m_pos < m_input.length() && peek() != '\n')
		advance();
	if (peek() == '\n')
		advance();
}

size_t	Lexer::isIPAddress(size_t start) const
{
	size_t	cpos = start;
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
