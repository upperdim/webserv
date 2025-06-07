/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 11:05:42 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/07 17:01:34 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "Parser.hpp"
#include "Config.hpp"

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


Config	Parser::parse(void)
{
	m_currentToken = m_lexer.nextToken();
	while (m_currentToken.type != TokenType::END_OF_INPUT)	// TODO: exit on ERROR as well ???
	{
		switch (m_currentToken.type)
		{
			case (TokenType::EVENTS):
			{
				LOG_INFO("\033[92mwebserv\033[94m doesn't support the \"EVENTS\" directive, skipping this directive.");
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
				consume(TokenType::OPEN_BRACE, "webserv: invalid number of arguments in \"http\" directive, OPEN_BRACE \"{\" expected");

				switch (m_currentToken.type)
				{
					case (TokenType::SERVER):
					{
						m_currentToken = m_lexer.nextToken();
						consume(TokenType::OPEN_BRACE, "webserv: invalid number of arguments in \"server\" directive, OPEN_BRACE \"{\" expected");
						ServerBlock serverBlock = parseServer();
						m_config.serverblocks.emplace_back(serverBlock);
						break ;
					}
					case (TokenType::URI):
						// Fall through
					case (TokenType::NUMBER):
						// Fall through
					case (TokenType::STRING):
						throw ( std::runtime_error("webserv: unknown or unsupported directive \"" + m_currentToken.value + "\".") );
					case (TokenType::END_OF_INPUT):
						throw ( std::runtime_error("webserv: unexpected end of file, expected `}'") );
					default:
						throw ( std::runtime_error("webserv: unexpected \"" + m_currentToken.getTokenValue() + "\".") );
				}

				break ;
			}
			default:
				throw ( std::runtime_error(std::string("webserv: unknown or unsupported directive \"") + m_currentToken.getTokenValue() +"\"." ) );
		}

		m_currentToken = m_lexer.nextToken();
	}

	return (m_config);
}


/* ************************************************************************** */
/* ************************************************************************** */


ServerBlock	Parser::parseServer(void)
{
	ServerBlock	serverBlock;

	while (m_currentToken.type != TokenType::CLOSE_BRACE && m_currentToken.type != TokenType::END_OF_INPUT && m_currentToken.type != TokenType::ERROR)
	{
		switch (m_currentToken.type)
		{
			case (TokenType::LISTEN):
				parseListenDirective(serverBlock);
				break ;
			default:
				throw ( std::runtime_error("webserv: unknown or unsupported directive \"" + m_currentToken.getTokenValue() + "\".") );
		}
		// m_currentToken = m_lexer.nextToken();		// TODO: if done this can go, til then i let it sit here
	}

	serverBlock.serverNames.emplace_back("whoops");	// TODO: change this
	return (serverBlock);
}

void	Parser::parseListenDirective(ServerBlock& serverBlock)
{
	m_currentToken = m_lexer.nextToken();

	switch (m_currentToken.type)
	{
		case (TokenType::HTTP):
			// Fall through
		case (TokenType::SERVER):
			// Fall through
		case (TokenType::LOCATION):
			// Fall through
		case (TokenType::URI):
			// Fall through
		case (TokenType::STRING):
			throw ( std::runtime_error("webserv: host not found in \"" + m_currentToken.getTokenValue() + "\"") );
		case (TokenType::OPEN_BRACE):
			throw ( std::runtime_error("webserve: directive \"listen\" is not terminated by \";\"") );
		case (TokenType::SEMICOLON):
			throw ( std::runtime_error("webserve: invalid number of arguments in \"listen\" directive") );
		case (TokenType::NUMBER):
			// TODO: decide if we support IP:PORT
			serverBlock.listenPort = validatePort(m_currentToken.value);
			m_currentToken = m_lexer.nextToken();
			ensureDirectiveTermination("listen");
			consume(TokenType::SEMICOLON, "webserv: invalid parameter \"" + m_currentToken.getTokenValue() + "\".");
			return ;
		default:
			throw ( std::runtime_error("webserv: unexpected \"" + m_currentToken.getTokenValue() + "\".") );
	}
}

void	Parser::consume(TokenType _type, std::string msg)
{
	if (m_currentToken.type != _type)
		throw ( std::runtime_error(msg) );
	m_currentToken = m_lexer.nextToken();
}

void	Parser::ensureDirectiveTermination(const std::string& name)
{
	if (m_lexer.precededByComment)
		throw ( std::runtime_error("directive \"" + name + "\" is not terminated by \";\"") );
}

unsigned int	Parser::validatePort(const std::string& _port)
{
	unsigned int	port = std::stoi(_port);
	if (port == 0 || port > 65535)
		throw ( std::runtime_error("invalid port in \"" + _port + "\" of the \"listen\" directive.") );

	return (port);
}

Config Parser::mockParseConfig(std::string configFilePath) {
	std::string configFile = readConfigFile(configFilePath);

	Config config;
	std::cout << configFile << std::endl;
	return config;
}

std::string Parser::readConfigFile(std::string configFilePath) {
	std::ifstream configFile(configFilePath);
	if (!configFile) {
		throw std::runtime_error("Config file is not found.");
	}

	std::stringstream buffer;
	buffer << configFile.rdbuf();
	configFile.close();

	return buffer.str();
}
