/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 11:05:42 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/11 10:26:17 by nmihaile         ###   ########.fr       */
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

	while (m_currentToken.type != TokenType::END_OF_INPUT) {	//	TODO: exit on INAVLID as well ???
		if (m_currentToken.type == TokenType::KEYWORD) {
			switch (m_currentToken.keywordType) {
				case KeywordType::EVENTS: {
					// Skipping EVENTS
					LOG_INFO("\033[92mwebserv\033[94m doesn't support the \"EVENTS\" directive, skipping this directive, " + m_currentToken.onLine());
					while (m_currentToken.type != TokenType::CLOSE_BRACE && m_currentToken.type != TokenType::END_OF_INPUT && m_currentToken.type != TokenType::INVALID)
						m_currentToken = m_lexer.nextToken();
					if (m_currentToken.type == TokenType::END_OF_INPUT)
						throw ( std::runtime_error("webserv: unexpected end of file, expected `}' after 'EVENTS' directive " + m_currentToken.onLine()) );
					else if (m_currentToken.type == TokenType::INVALID)
						throw ( std::runtime_error("webserv: an error occured while skipping unsupported 'EVENTS' directive " + m_currentToken.onLine()) );
					break ;
				}
				case KeywordType::HTTP: {
					m_currentToken = m_lexer.nextToken();
					consume(TokenType::OPEN_BRACE, "webserv: invalid number of arguments in \"http\" directive, OPEN_BRACE \"{\" expected " + m_currentToken.onLine());

					if (m_currentToken.type == TokenType::KEYWORD && m_currentToken.keywordType == KeywordType::SERVER) {
						// Parsing SERVER
						m_currentToken = m_lexer.nextToken();
						consume(TokenType::OPEN_BRACE, "webserv: invalid number of arguments in \"server\" directive, OPEN_BRACE \"{\" expected " + m_currentToken.onLine());
						ServerBlock serverBlock = parseServer();
						m_config.serverBlocks.emplace_back(serverBlock);
					} else if (m_currentToken.type == TokenType::PARAM ||
							m_currentToken.type == TokenType::URI ||
							m_currentToken.type == TokenType::NUMBER) {
						throw ( std::runtime_error("webserv: unknown or unsupported directive \"" + m_currentToken.value + "\", " + m_currentToken.onLine()) );
					} else if (m_currentToken.type == TokenType::END_OF_INPUT) {
						throw ( std::runtime_error("webserv: unexpected end of file, expected \"}\" " + m_currentToken.onLine()) );
					} else {
						throw ( std::runtime_error("webserv: unexpected \"" + m_currentToken.getTokenValue() + "\" " + m_currentToken.onLine()) );
					}
				}
				default:
					throw ( std::runtime_error(std::string("webserv: unknown or unsupported directive \"") + m_currentToken.getTokenValue() +"\", " + m_currentToken.onLine()) );
			}
		} else {
			throw ( std::runtime_error(std::string("webserv: unknown or unsupported directive \"") + m_currentToken.getTokenValue() +"\", " + m_currentToken.onLine()) );
		}

		m_currentToken = m_lexer.nextToken();
	}

	return m_config;
}


/* ************************************************************************** */
/* ************************************************************************** */


ServerBlock	Parser::parseServer(void)
{
	ServerBlock	serverBlock;

	while (m_currentToken.type != TokenType::CLOSE_BRACE && m_currentToken.type != TokenType::END_OF_INPUT && m_currentToken.type != TokenType::INVALID) {
		if (m_currentToken.type == TokenType::KEYWORD) {
			switch (m_currentToken.keywordType) {
				case KeywordType::LISTEN:
					parseListenDirective(serverBlock);
					break ;
				default:
					throw ( std::runtime_error("webserv: unknown or unsupported directive \"" + m_currentToken.getTokenValue() + "\", " + m_currentToken.onLine()) );
			}
		}
		else
			throw ( std::runtime_error("webserv: unknown or unsupported directive \"" + m_currentToken.getTokenValue() + "\", " + m_currentToken.onLine()) );	// TODO: check if thiss error is correct I suppose it is not
		// m_currentToken = m_lexer.nextToken();		//	TODO: if done this can go, til then i let it sit here
	}

	serverBlock.serverNames.emplace_back("whoops");		//	TODO: change this
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
			throw ( std::runtime_error("webserv: host not found in \"" + m_currentToken.getTokenValue() + "\", " + m_currentToken.onLine()) );
		case (TokenType::STRING):
		{
			LOG_INFO(std::string("Resolving Token::STRING : \033[0m" + m_currentToken.getTokenValue() + " \033[94m" + m_currentToken.onLine()));
			addrinfo	hints;
			hints.ai_family = AF_INET;			// IPv4
			hints.ai_socktype = SOCK_STREAM;	// we want a: stream socket (typically used for TCP)
			addrinfo*	res;					// result
			if (getaddrinfo(m_currentToken.value.c_str(), nullptr, &hints, &res) != 0)
				throw ( std::runtime_error("webserv: host not found in \"" + m_currentToken.getTokenValue() + "\", " + m_currentToken.onLine()) );
			
			sockaddr_in* ipv4 = reinterpret_cast<sockaddr_in*>(res->ai_addr);
			serverBlock.host = ipv4->sin_addr.s_addr;
			freeaddrinfo(res);
			parsePortAfterHost(serverBlock);
			return ;
		}
		case (TokenType::OPEN_BRACE):
			throw ( std::runtime_error("webserve: directive \"listen\" is not terminated by \";\", " + m_currentToken.onLine()) );
		case (TokenType::SEMICOLON):
			throw ( std::runtime_error("webserve: invalid number of arguments in \"listen\" directive, " + m_currentToken.onLine()) );
		case (TokenType::NUMBER):
		{
			// TODO: support STRING for domains as incomming host	==> CHECK we support it!
			serverBlock.host = INADDR_ANY;
			serverBlock.listenPort = validatePort(m_currentToken.value);
			m_currentToken = m_lexer.nextToken();
			ensureDirectiveTermination("listen");
			consume(TokenType::SEMICOLON, "webserv: invalid parameter \"" + m_currentToken.getTokenValue() + "\", " + m_currentToken.onLine());
			return ;
		}
		case (TokenType::IP):
		{
			in_addr	addr;
			int result = inet_pton(AF_INET, m_currentToken.value.c_str(), &addr);
			if (result == 0)
				throw ( std::runtime_error("webserv: invalid IP address \"" + m_currentToken.getTokenValue() + "\", " + m_currentToken.onLine()) );

			serverBlock.host = addr.s_addr;
			parsePortAfterHost(serverBlock);
			return ;
		}
		default:
			throw ( std::runtime_error("webserv: unexpected \"" + m_currentToken.getTokenValue() + "\"," + m_currentToken.onLine()) );
	}
}

void	Parser::parsePortAfterHost(ServerBlock& serverBlock)
{
	m_currentToken = m_lexer.nextToken();
	if (m_currentToken.type == TokenType::COLON)
	{
		m_currentToken = m_lexer.nextToken();
		if (m_currentToken.type != TokenType::NUMBER)
			throw ( std::runtime_error("webserv: invalid port \"" + m_currentToken.getTokenValue() + "\", " + m_currentToken.onLine()) );
		serverBlock.listenPort = validatePort(m_currentToken.value);
		m_currentToken = m_lexer.nextToken();
	}
	else
		serverBlock.listenPort = 80;
	ensureDirectiveTermination("listen");
	consume(TokenType::SEMICOLON, "webserv: invalid parameter \"" + m_currentToken.getTokenValue() + "\", " + m_currentToken.onLine());
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
		throw ( std::runtime_error("directive \"" + name + "\" is not terminated by \";\", " + m_currentToken.onLine()) );
}

unsigned int	Parser::validatePort(const std::string& _port)
{
	unsigned int	port = std::stoi(_port);
	if (port == 0 || port > 65535)
		throw ( std::runtime_error("invalid port in \"" + _port + "\" of the \"listen\" directive, " + m_currentToken.onLine()) );

	return (port);
}


/* ************************************************************************** */
/* TODO: DELETE LATER                                                         */
/* ************************************************************************** */


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
