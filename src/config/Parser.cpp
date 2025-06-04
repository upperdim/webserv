/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 11:05:42 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/04 19:30:26 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

Parser::Parser(const Lexer& _lexer)
	:	m_lexer(_lexer)
{
}

Parser::~Parser()
{
}


/* ************************************************************************** */
/* ************************************************************************** */


std::string	Parser::parse()	// TODO: return config struct
{
	// m_file_path = _file_path;
	// m_ifs.open(m_file_path);

	// if (!m_ifs.is_open())
	// 	throw ( std::invalid_argument(std::string("failed to open file: ") + m_file_path) );

	// m_ifs.close();


	// read all tokens and log them out
	Token currentToken = m_lexer.nextToken();
	while (currentToken.type != TokenType::END_OF_INPUT && currentToken.type != TokenType::ERROR)
	{
		LOG_INFO( currentToken.toString() );
		currentToken = m_lexer.nextToken();
	}
	LOG_INFO( currentToken.toString() );


	// return (m_config);
	return (std::string(""));
}
