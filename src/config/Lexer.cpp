/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 11:38:25 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/10 19:09:02 by nmihaile         ###   ########.fr       */
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


Token	Lexer::nextToken(bool _precededByComment)
{
	precededByComment = _precededByComment;
	skipWhitespaces();

	if (m_pos >= m_input.length())
		return Token(TokenType::END_OF_INPUT, m_line);

	markStart();

	char c = peek();

	if (c == '#') {
		skipComment();
		skipWhitespaces();
		if (m_pos >= m_input.length())
			return Token(TokenType::END_OF_INPUT, m_line);
		return nextToken(true);
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
	}

	//	Handle numbers
	if (std::isdigit(c)) {
		if (isMixedAlphanumeric())
			return readAndClassify();	//	Domain/address starting with digit
		return readNumber();
	}

	//	Handle keywords and general parameters
	if (std::isalpha(c) || c == '/') {
		return readAndClassify();
	}

	advance();
	return Token(TokenType::INVALID, std::string(1, c), m_line);
}


/* ************************************************************************** */
/* ************************************************************************** */


void	Lexer::markStart(void)
{
	m_startPos = m_pos;
}

char	Lexer::peek(size_t offset) const
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

std::string Lexer::getCurrentLexeme() const
{
	return m_input.substr(m_startPos, m_pos - m_startPos);
}

KeywordType	Lexer::getKeywordType(const std::string& _keyword) const
{
	if (directives.find(_keyword) != directives.end()) {
		return directives.at(_keyword);
	}
	return KeywordType::NONE;
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

bool	Lexer::isMixedAlphanumeric() const
{
	size_t	lookAhead = 0;

	// skip inital digits
	while (m_pos + lookAhead < m_input.length() && std::isdigit(peek(lookAhead)))
		++lookAhead;
	
	//	check if followed by IPv4 domain/address aka isalpha, '-', '.'
	if (m_pos + lookAhead < m_input.length()) {
		char nextChar = peek(lookAhead);
		return (std::isalpha(nextChar) || nextChar == '-' || nextChar == '.');
	}

	return false;
}

Token	Lexer::readNumber()
{
	while (m_pos < m_input.length() && (std::isdigit(peek()) || peek() == '.'))
		advance();
	return Token(TokenType::NUMBER, getCurrentLexeme(), m_line);
}

Token	Lexer::readAndClassify()
{
	while (m_pos < m_input.length() &&
			(std::isalnum(peek()) || peek() == '.' || peek() == '/' || peek() == '_' || peek() == '-')) {
		advance();
	}

	std::string lexeme = getCurrentLexeme();

	// check if it is a supported directive aka KEYWORD or URI
	KeywordType keywordType = getKeywordType(lexeme);
	if (keywordType != KeywordType::NONE) {
		return Token(TokenType::KEYWORD, keywordType, lexeme, m_line);
	} else if (lexeme[0] == '/') {
		return Token(TokenType::URI, lexeme, m_line);
	}

	// By default we return a PARAM
	return Token(TokenType::PARAM, lexeme, m_line);
}

// size_t	Lexer::isIPAddress(size_t start) const
// {
// 	size_t	cpos = start;
// 	int		octetCount = 0;
// 	int		currentOctet;

// 	if (cpos + 7 > m_input.length())
// 		return (0);

// 	while (cpos < m_input.length() && octetCount < 4)
// 	{
// 		currentOctet = 0;
// 		int digits = 0;
		
// 		while (cpos < m_input.length() && std::isdigit(m_input[cpos]) && digits < 3)
// 		{
// 			currentOctet = currentOctet * 10 + (m_input[cpos++] - '0');
// 			++digits;
// 		}

// 		if (currentOctet > 255)
// 			return (0);
		
// 		if (octetCount < 3)
// 		{
// 			if (cpos >= m_input.length() || m_input[cpos] != '.')
// 				return (0);
// 			++cpos;
// 		}

// 		++octetCount;
// 	}

// 	if ( octetCount == 4 && (cpos == m_input.length() || std::isdigit(m_input[cpos]) == false) )
// 		return (cpos);
// 	return (0);
// }
