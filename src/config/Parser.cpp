/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 11:05:42 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/06 11:07:06 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

Parser::Parser(const Lexer& _lexer)
	:	m_lexer(_lexer),
		m_currentToken(TokenType::END_OF_INPUT, "")
{
}

Parser::~Parser()
{
}


/* ************************************************************************** */
/* ************************************************************************** */


std::string	Parser::parse()	// TODO: return config struct
{

	// DEBUG: => READ ALL TOKENS AND LOG THEM OUT
	// m_currentToken = m_lexer.nextToken();
	// while (m_currentToken.type != TokenType::END_OF_INPUT && m_currentToken.type != TokenType::ERROR)
	// {
	// 	LOG_INFO( m_currentToken.toString() );
	// 	m_currentToken = m_lexer.nextToken();
	// }
	// LOG_INFO( m_currentToken.toString() );


	m_currentToken = m_lexer.nextToken();
	while (m_currentToken.type != TokenType::END_OF_INPUT)
	{
		switch (m_currentToken.type)
		{
			case (TokenType::EVENTS):
			{
				LOG_INFO("\033[92mwebserv\033[94m doesn't support the 'EVENTS' directive.");
				while (m_currentToken.type != TokenType::CLOSE_BRACE && m_currentToken.type != TokenType::END_OF_INPUT && m_currentToken.type != TokenType::ERROR)
					m_currentToken = m_lexer.nextToken();
				if (m_currentToken.type == TokenType::END_OF_INPUT)
					throw ( std::runtime_error("webserv: unexpected end of file, expected `}' after 'EVENTS' directive.") );
				else if (m_currentToken.type == TokenType::ERROR)
					throw ( std::runtime_error("webserv: an error occured while skipping unsupported 'EVENTS' directive.") );
				break ;
			}
			case (TokenType::HTTP):
			{
				m_currentToken = m_lexer.nextToken();
				consume(TokenType::OPEN_BRACE, "webserv: invalid number of arguments in \"http\" directive, OPEN_BRACE `{' expected");

				///////////////////////////////////////////////
				//////////////continue here////////////////////
				///////////////////////////////////////////////
		
				break ;
			}
			default:
				throw ( std::runtime_error(std::string("webserv: unknown directive ") + m_currentToken.value ) );
		}
		
		m_currentToken = m_lexer.nextToken();
	}

	// return (m_config);
	return (std::string(""));
}


/* ************************************************************************** */
/* ************************************************************************** */


void	Parser::consume(TokenType _type, std::string msg)
{
	if (m_currentToken.type != _type)
		throw ( std::runtime_error(msg) );
	m_currentToken = m_lexer.nextToken();
}
