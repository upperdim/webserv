/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser2.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 11:02:33 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/14 10:38:39 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser2.hpp"

Parser::Parser(const std::vector<Token>& _tokens)
	:	m_tokens(_tokens),
		m_pos(0)
{
}

Parser::~Parser()
{
}


/* ************************************************************************** */
/* ************************************************************************** */


Config	Parser::parse(void)
{
	Config config = {};

	while (m_pos < m_tokens.size() && !isAtEnd()) {
		// expecting a KEYWORD or throw accordingly
		if (peek().type == TokenType::SEMICOLON ||
			peek().type == TokenType::OPEN_BRACE ||
			peek().type == TokenType::CLOSE_BRACE)
			throw_Unexpected(peek());
		if (peek().type != TokenType::KEYWORD)
			throw_UnknownOrUnsupportedDirective(peek());

		const Token& directive = peek();
		switch (directive.keywordType) {
			case KeywordType::EVENTS:
				skipEventsDirective();
				break ;
			default:
				break ;
		}
		advance();	//	TODO: delete me later if not needed!!!!
	}

	return config;
}


/* ************************************************************************** */
/* ************************************************************************** */


const Token&	Parser::prev(void) const
{
	if (m_pos >= 1)
		return m_tokens[m_pos - 1];
	return m_tokens[0];
}

const Token&	Parser::peek(void) const
{
	return m_tokens[m_pos];
}

const Token&	Parser::advance(void)
{
	return m_tokens[m_pos++];
}

bool	Parser::isAtEnd(void) const
{
	return m_pos < m_tokens.size() && m_tokens[m_pos].type == TokenType::END_OF_INPUT;
}

void	Parser::expect(TokenType _type, const std::string& msg)
{
	if (m_pos >= m_tokens.size() || m_tokens[m_pos].type != _type)
		throw_SyntaxError(msg);
	advance();
}


void	Parser::skipEventsDirective(void)
{
	advance();	//	consumes "EVENTS"
	
	// throw if params available
	if (peek().type == TokenType::PARAM ||
	    peek().type == TokenType::URI ||
	    peek().type == TokenType::NUMBER)
		throw_InvalidNumberOfArguments(prev());
		
	expect(TokenType::OPEN_BRACE, "expected \"{\"");

	// count scopes and skips any tokens inside EVENTS directive
	size_t braceCount = 1;
	while (m_pos < m_tokens.size() && !isAtEnd()) {
		if (peek().type == TokenType::OPEN_BRACE)
			++braceCount;
		if (peek().type == TokenType::CLOSE_BRACE)
		{
			if (braceCount == 1)
				break ;
			--braceCount;
		}
		advance();
	}

	expect(TokenType::CLOSE_BRACE, "expected \"}\"");
}


/* ************************************************************************** */
/* ************************************************************************** */


void	Parser::throw_SyntaxError(const std::string& msg) const
{
	throw std::runtime_error("Syntax error" + (m_pos < m_tokens.size() ? m_tokens[m_pos].inLine() : m_tokens.back().inLine()) + RED + (msg.empty() ? "" : ": " + msg));
}

void	Parser::throw_Unexpected(const Token& token) const
{
	throw std::runtime_error("unexpected \"" + token.getTokenValue() + "\"" + token.inLine());
}

void	Parser::throw_UnknownOrUnsupportedDirective(const Token& token) const
{
	throw std::runtime_error("unknown or unsupported directive \"" + token.getTokenValue() + "\"" + token.inLine());
}

void	Parser::throw_InvalidNumberOfArguments(const Token& token) const
{
	throw std::runtime_error("invalid number of arguments in \"" + token.getTokenValue() + "\" directive" + token.inLine());
}
