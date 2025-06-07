/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 11:05:43 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/07 15:56:04 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <fstream>
#include <stdexcept>
#include "Token.hpp"
#include "Lexer.hpp"
#include "Log.hpp"

// TODO: THIS IS TEMP
#include <vector>
struct ServerBlock
{
	int							listenPort;
	std::vector<std::string>	serverNames;
};

struct Config
{
	std::vector<ServerBlock>	serverblocks;
};


class Parser
{
public:
	Parser(const Lexer& _lexer);
	~Parser();

	Config	parse(void);

private:
	Parser(const Parser& other);
	Parser&	operator=(const Parser& rhs);

	ServerBlock		parseServer(void);
	void			parseListenDirective(ServerBlock& serverBlock);
	void			consume(TokenType _type, std::string msg = "");
	void			ensureDirectiveTermination(const std::string& name);

	unsigned int	validatePort(const std::string& _port);

	Lexer	m_lexer;
	Token	m_currentToken;
	Config	m_config;
};

#endif
