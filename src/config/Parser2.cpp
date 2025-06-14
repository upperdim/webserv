/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser2.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 11:02:33 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/14 09:20:19 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser2.hpp"

Parser::Parser(const std::vector<Token>& _tokens)
	:	m_tokens(_tokens)
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
	(void)m_tokens;
	return config;
}


/* ************************************************************************** */
/* ************************************************************************** */


const Token&	Parser::peek(void) const
{
	return m_tokens[m_pos];
}

const Token&	Parser::advance(void)
{
	return m_tokens[m_pos++];
}
