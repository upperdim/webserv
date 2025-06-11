/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 11:05:42 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/11 16:29:12 by nmihaile         ###   ########.fr       */
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
				case KeywordType::EVENTS:
					parseEvents();
					break ;
				case KeywordType::HTTP:
					parseHttp();
					break ;
				default:
					throw ( std::runtime_error(std::string("webserv: \"" + m_currentToken.getTokenValue() + "\" directive is not allowed here: ") + m_currentToken.onLine()) );
			}
		} else {
			throw ( std::runtime_error(std::string("webserv: unknown or unsupported directive \"") + m_currentToken.getTokenValue() +"\", " + m_currentToken.onLine()) );
		}
		// m_currentToken = m_lexer.nextToken();	//	TODO:	delete me => we expect a new token here from the dedicated parseDirective functions
	}

	return m_config;
}


/* ************************************************************************** */
/* ************************************************************************** */


void	Parser::parseEvents(void)
{
	// Skipping EVENTS
	LOG_INFO("\033[92mwebserv\033[94m doesn't support the \"EVENTS\" directive, skipping this directive, " + m_currentToken.onLine());
	while (m_currentToken.type != TokenType::CLOSE_BRACE && m_currentToken.type != TokenType::END_OF_INPUT && m_currentToken.type != TokenType::INVALID)
		m_currentToken = m_lexer.nextToken();
	if (m_currentToken.type == TokenType::END_OF_INPUT)
		throw ( std::runtime_error("webserv: unexpected end of file, expected `}' after 'EVENTS' directive " + m_currentToken.onLine()) );
	else if (m_currentToken.type == TokenType::INVALID)
		throw ( std::runtime_error("webserv: an error occured while skipping unsupported 'EVENTS' directive " + m_currentToken.onLine()) );
	consume(TokenType::CLOSE_BRACE, "webserv: unexpected \"" + m_currentToken.getTokenValue() + "\" " + m_currentToken.onLine());
}

void	Parser::parseHttp(void)
{
	m_currentToken = m_lexer.nextToken();
	consume(TokenType::OPEN_BRACE, "webserv: invalid number of arguments in \"http\" directive, OPEN_BRACE \"{\" expected " + m_currentToken.onLine());

	while (m_currentToken.type == TokenType::KEYWORD && m_currentToken.keywordType == KeywordType::SERVER) {
		// Parsing SERVER
		m_currentToken = m_lexer.nextToken();
		consume(TokenType::OPEN_BRACE, "webserv: invalid number of arguments in \"server\" directive, OPEN_BRACE \"{\" expected " + m_currentToken.onLine());
		ServerBlock serverBlock = parseServer();
		m_config.serverBlocks.emplace_back(serverBlock);
	}

	if (m_currentToken.type == TokenType::PARAM ||
	    m_currentToken.type == TokenType::URI ||
	    m_currentToken.type == TokenType::NUMBER) {
		throw ( std::runtime_error("webserv: unknown or unsupported directive \"" + m_currentToken.value + "\", " + m_currentToken.onLine()) );
	} else if (m_currentToken.type == TokenType::END_OF_INPUT) {
		throw ( std::runtime_error("webserv: unexpected end of file, expected \"}\" " + m_currentToken.onLine()) );
	}
	// } else {
	// 	throw ( std::runtime_error("webserv: unexpected \"" + m_currentToken.getTokenValue() + "\" " + m_currentToken.onLine()) );
	// }
	consume(TokenType::CLOSE_BRACE, "webserv: unexpected \"" + m_currentToken.getTokenValue() + "\" " + m_currentToken.onLine());
}

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
		// m_currentToken = m_lexer.nextToken();	//	TODO:	delete me => we expect a new token here from the dedicated parseDirective functions
	}

	consume(TokenType::CLOSE_BRACE, "webserv: unexpected \"" + m_currentToken.getTokenValue() + "\" " + m_currentToken.onLine());		//	TODO:	better error msg

	serverBlock.serverNames.emplace_back("whoops");		//	TODO: change this
	return (serverBlock);
}

void	Parser::parseListenDirective(ServerBlock& serverBlock)
{
	m_currentToken = m_lexer.nextToken();

	switch (m_currentToken.type) {
		case TokenType::KEYWORD:
			// Fall through
		case TokenType::URI:
			throw ( std::runtime_error("webserv: host not found in \"" + m_currentToken.getTokenValue() + "\", " + m_currentToken.onLine()) );
		case TokenType::PARAM: {
			//	check for DomainName or IP
			LOG_INFO(std::string("Resolving Token::PARAM: \033[0m" + m_currentToken.getTokenValue() + " \033[94m" + m_currentToken.onLine()));	//	TODO:	delete me
	
			if (m_currentToken.value == "localhost")
				m_currentToken.value = "127.0.0.1";

			if (Validator::isIPAddr(m_currentToken.value)) {
				in_addr	addr;
				int result = inet_pton(AF_INET, m_currentToken.value.c_str(), &addr);
				if (result == 0)
					throw ( std::runtime_error("webserv: invalid IP address \"" + m_currentToken.getTokenValue() + "\", " + m_currentToken.onLine()) );

				serverBlock.host = addr.s_addr;
			} else if (Validator::isDomainName(m_currentToken.value)) {
				addrinfo	hints;
				hints.ai_family = AF_INET;			// IPv4
				hints.ai_socktype = SOCK_STREAM;	// we want a: stream socket (typically used for TCP)
				addrinfo*	res;					// result
				if (getaddrinfo(m_currentToken.value.c_str(), nullptr, &hints, &res) != 0)
					throw ( std::runtime_error("webserv: host not found in \"" + m_currentToken.getTokenValue() + "\", " + m_currentToken.onLine()) );
				
				sockaddr_in* ipv4 = reinterpret_cast<sockaddr_in*>(res->ai_addr);
				serverBlock.host = ipv4->sin_addr.s_addr;
				freeaddrinfo(res);
			} else
				throw ( std::runtime_error("webserve:invalid port in \"" + m_currentToken.getTokenValue() + "\" of the \"listen\" directive, " + m_currentToken.onLine()) );

			//	parse Port after Host
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
			return ;
		}
		case TokenType::NUMBER: {
			//	parse Port
			serverBlock.host = INADDR_ANY;
			serverBlock.listenPort = validatePort(m_currentToken.value);
			m_currentToken = m_lexer.nextToken();
			ensureDirectiveTermination("listen");
			consume(TokenType::SEMICOLON, "webserv: invalid parameter \"" + m_currentToken.getTokenValue() + "\", " + m_currentToken.onLine());
			return ;
		}
		case (TokenType::COLON):
			throw ( std::runtime_error("webserve:invalid port in \"" + m_currentToken.getTokenValue() + "\" of the \"listen\" directive, " + m_currentToken.onLine()) );
		case (TokenType::SEMICOLON):
			throw ( std::runtime_error("webserve: invalid number of arguments in \"listen\" directive, " + m_currentToken.onLine()) );
		case (TokenType::OPEN_BRACE):
			throw ( std::runtime_error("webserve: directive \"listen\" is not terminated by \";\", " + m_currentToken.onLine()) );
		default:
			throw ( std::runtime_error("webserv: unexpected \"" + m_currentToken.getTokenValue() + "\"," + m_currentToken.onLine()) );
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
