/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 11:05:42 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/12 10:58:22 by nmihaile         ###   ########.fr       */
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
	bool parsedHTTP = false;

	while (m_currentToken.type != TokenType::END_OF_INPUT) {	//	TODO: exit on INAVLID as well ???
		if (m_currentToken.type == TokenType::KEYWORD) {
			switch (m_currentToken.keywordType) {
				case KeywordType::EVENTS:
					parseEvents();
					break ;
				case KeywordType::HTTP:
					if (parsedHTTP)
						throw_DirectiveIsDuplicate("HTTP");
					parseHttp();
					parsedHTTP = true;
					break ;
				default:
					throw_DirectiveIsNotAllowed(m_currentToken.getTokenValue());
			}
		} else {
			throw_UnknownOrUnsupportedDirective(m_currentToken.getTokenValue());
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
	LOG_INFO("\033[92mwebserv\033[94m doesn't support the \"EVENTS\" directive, skipping this directive, " + m_currentToken.inLine());
	while (m_currentToken.type != TokenType::CLOSE_BRACE && m_currentToken.type != TokenType::END_OF_INPUT) {
		// we also skip INVALID TOKENS now
		m_currentToken = m_lexer.nextToken();
	}
	if (m_currentToken.type == TokenType::END_OF_INPUT)
		throw_UnexpectedEndOfFile("}", "EVENTS");
	consume(TokenType::CLOSE_BRACE, ThrowType::UNEXPECTED);
}

void	Parser::parseHttp(void)
{
	m_currentToken = m_lexer.nextToken();
	consume(TokenType::OPEN_BRACE, ThrowType::INVALID_NUMBER_OF_ARGUMENTS, "HTTP");

	while (m_currentToken.type == TokenType::KEYWORD && m_currentToken.keywordType == KeywordType::SERVER) {
		// Parsing SERVER
		m_currentToken = m_lexer.nextToken();
		consume(TokenType::OPEN_BRACE, ThrowType::INVALID_NUMBER_OF_ARGUMENTS, "SERVER");
		ServerBlock serverBlock = parseServer();
		m_config.serverBlocks.emplace_back(serverBlock);
	}

	if (m_currentToken.type == TokenType::PARAM ||
	    m_currentToken.type == TokenType::URI ||
	    m_currentToken.type == TokenType::NUMBER) {
		throw_UnknownOrUnsupportedDirective(m_currentToken.value);
	} else if (m_currentToken.type == TokenType::END_OF_INPUT) {
		throw_UnexpectedEndOfFile("}", "HTTP");
	}
	//	TOTO:	delete me afte we checked: CAN WE SAFELY DELETE THIS
	// } else {		
	// 	throw ( std::runtime_error("webserv: unexpected \"" + m_currentToken.getTokenValue() + "\" " + m_currentToken.inLine()) );
	// }
	consume(TokenType::CLOSE_BRACE, ThrowType::UNEXPECTED);
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
				case KeywordType::SERVER_NAME:
					parseServerNameDirective(serverBlock);
					break ;
				case KeywordType::ERROR_PAGE:
					parseErrorPageDirective(serverBlock);
					break ;
				default:
					throw_UnknownOrUnsupportedDirective(m_currentToken.getTokenValue());
			}
		}
		else
			throw_UnknownOrUnsupportedDirective(m_currentToken.getTokenValue());	// TODO: check if thiss error is correct I suppose it is not
		// m_currentToken = m_lexer.nextToken();	//	TODO:	delete me => we expect a new token here from the dedicated parseDirective functions
	}

	consume(TokenType::CLOSE_BRACE, ThrowType::UNEXPECTED);		//	TODO:	better error msg
	return (serverBlock);
}

void	Parser::parseListenDirective(ServerBlock& serverBlock)
{
	m_currentToken = m_lexer.nextToken();

	switch (m_currentToken.type) {
		case TokenType::KEYWORD:
			// Fall through
		case TokenType::URI:
			throw_HostNotFound();
		case TokenType::PARAM: {
			//	check for DomainName or IP
			LOG_INFO(std::string("Resolving Token::PARAM: \033[0m" + m_currentToken.getTokenValue() + " \033[94m" + m_currentToken.inLine()));	//	TODO:	delete me
	
			if (m_currentToken.value == "localhost")
				m_currentToken.value = "127.0.0.1";

			if (Validator::isIPAddr(m_currentToken.value)) {
				in_addr	addr;
				int result = inet_pton(AF_INET, m_currentToken.value.c_str(), &addr);
				if (result == 0)
					throw_InvalidIPAddr();

				serverBlock.host = addr.s_addr;
			} else if (Validator::isDomainName(m_currentToken.value)) {
				addrinfo	hints;
				hints.ai_family = AF_INET;			// IPv4
				hints.ai_socktype = SOCK_STREAM;	// we want a: stream socket (typically used for TCP)
				addrinfo*	res;					// result
				if (getaddrinfo(m_currentToken.value.c_str(), nullptr, &hints, &res) != 0)
					throw_HostNotFound();
				
				sockaddr_in* ipv4 = reinterpret_cast<sockaddr_in*>(res->ai_addr);
				serverBlock.host = ipv4->sin_addr.s_addr;
				freeaddrinfo(res);
			} else
				throw_InvalidPort("LISTEN");

			//	parse Port after Host
			m_currentToken = m_lexer.nextToken();
			if (m_currentToken.type == TokenType::COLON)
			{
				m_currentToken = m_lexer.nextToken();
				if (m_currentToken.type != TokenType::NUMBER)
					throw_InvalidPort("LISTEN");
				
				unsigned int port;
				if (!Validator::isValidPort(m_currentToken.value, port))
					throw_InvalidPort("LISTEN");
				serverBlock.listenPort = port;

				m_currentToken = m_lexer.nextToken();
			}
			else
				serverBlock.listenPort = 80;

			ensureDirectiveTermination("listen");
			consume(TokenType::SEMICOLON, ThrowType::INVALID_PARAMETER);
			return ;
		}
		case TokenType::NUMBER: {
			//	parse Port
			serverBlock.host = INADDR_ANY;

			unsigned int port;
			if (!Validator::isValidPort(m_currentToken.value, port))
				throw_InvalidPort("LISTEN");
			serverBlock.listenPort = port;

			m_currentToken = m_lexer.nextToken();
			ensureDirectiveTermination("listen");
			consume(TokenType::SEMICOLON, ThrowType::INVALID_PARAMETER);
			return ;
		}
		case (TokenType::COLON):
			throw_InvalidPort("LISTEN");
		case (TokenType::SEMICOLON):
			throw_InvalidNumberOfArguments("LISTEN");
		case (TokenType::OPEN_BRACE):
			throw_DirectiveIsNotTerminated("LISTEN");
		default:
			throw_Unexpected();
	}
}

void	Parser::parseServerNameDirective(ServerBlock& serverBlock)
{
	m_currentToken = m_lexer.nextToken();

	while (m_currentToken.type == TokenType::PARAM) {
		if (Validator::isValidServerName(m_currentToken.value))
			serverBlock.serverNames.emplace_back(m_currentToken.value);
		else
			throw_AccpetsOnlyDomainNames();
		m_currentToken = m_lexer.nextToken();
	}

	if (m_currentToken.type == TokenType::KEYWORD ||
	    m_currentToken.type == TokenType::URI ||
	    m_currentToken.type == TokenType::NUMBER ||
	    m_currentToken.type == TokenType::INVALID ||
	    m_currentToken.type == TokenType::COLON)
			throw_AccpetsOnlyDomainNames();

	if (m_currentToken.type == TokenType::OPEN_BRACE)
		throw_DirectiveIsNotTerminated("server_name");
	if (m_currentToken.type == TokenType::CLOSE_BRACE)
		throw_Unexpected();

	consume(TokenType::SEMICOLON, ThrowType::INVALID_PARAMETER);
}

void	Parser::parseErrorPageDirective(ServerBlock& serverBlock)
{
	m_currentToken = m_lexer.nextToken();

	if (m_currentToken.type == TokenType::SEMICOLON)
		throw_InvalidNumberOfArguments("error_page");
	if (m_currentToken.type == TokenType::OPEN_BRACE)
		throw_DirectiveIsNotTerminated("error_page");
	if (m_currentToken.type == TokenType::CLOSE_BRACE)
		throw_Unexpected();
	if (m_currentToken.type != TokenType::NUMBER)
		throw_InvalidValue();
	int	error_nbr = std::stoi(m_currentToken.value);

	m_currentToken = m_lexer.nextToken();
	if (m_currentToken.type == TokenType::SEMICOLON)
		throw_InvalidNumberOfArguments("error_page");
	if (m_currentToken.type == TokenType::OPEN_BRACE)
		throw_DirectiveIsNotTerminated("error_page");
	if (m_currentToken.type == TokenType::CLOSE_BRACE)
		throw_Unexpected();
	if (m_currentToken.type != TokenType::URI)
		throw_InvalidValue();
	
	serverBlock.errorPagePaths[error_nbr] = m_currentToken.value;

	m_currentToken = m_lexer.nextToken();
	consume(TokenType::SEMICOLON, ThrowType::INVALID_PARAMETER);
}

void	Parser::consume(TokenType _type, ThrowType _throwType, std::string directive)
{
	if (m_currentToken.type != _type) {
		switch (_throwType) {
			case ThrowType::UNEXPECTED:
				throw_Unexpected();
			case ThrowType::INVALID_NUMBER_OF_ARGUMENTS:
				throw_InvalidNumberOfArguments(directive);
			case ThrowType::INVALID_PARAMETER:
				throw_InvalidParameter();
			default:
				throw std::runtime_error("error consuming token \"" + m_currentToken.getTokenValue() + "\"" + m_currentToken.inLine());
		}
	}
	m_currentToken = m_lexer.nextToken();
}

void	Parser::ensureDirectiveTermination(const std::string& directive)
{
	if (m_lexer.precededByComment)
		throw_DirectiveIsNotTerminated(directive);
}


/* ************************************************************************** */
/* THROW_ERROR_METHODS                                                        */
/* ************************************************************************** */


void	Parser::throw_DirectiveIsNotAllowed(const std::string& directive) const
{
	throw std::runtime_error("\"" + directive + "\" directive is not allowed" + m_currentToken.inLine());
}

void	Parser::throw_UnknownOrUnsupportedDirective(const std::string& directive) const
{
	throw std::runtime_error("unknown or unsupported directive \"" + directive + "\"" + m_currentToken.inLine());
}

void	Parser::throw_UnexpectedEndOfFile(const std::string& expected, const std::string& directive) const
{
	throw std::runtime_error("webserv: unexpected end of file, expected \"" + expected + "\" after \"" + directive + "\" directive" + m_currentToken.inLine());
}

void	Parser::throw_DirectiveIsDuplicate(const std::string& directive) const
{
	throw std::runtime_error("\"" + directive + "\" directive is duplicate" + m_currentToken.inLine());
}

void	Parser::throw_InvalidIPAddr(void) const
{
	throw std::runtime_error("invalid IP address \"" + m_currentToken.getTokenValue() + "\"" + m_currentToken.inLine());
}

void	Parser::throw_HostNotFound(void) const
{
	throw std::runtime_error("host not found in \"" + m_currentToken.getTokenValue() + "\"" + m_currentToken.inLine());
}

void	Parser::throw_InvalidPort(const std::string& directive) const
{
	throw std::runtime_error("invalid port in \"" + m_currentToken.getTokenValue() + "\" of the \"" + directive + "\" directive" + m_currentToken.inLine());
}

void	Parser::throw_AccpetsOnlyDomainNames(void) const
{
	throw std::runtime_error("accpets only \"domain names\" as server_name \"" + m_currentToken.getTokenValue() + "\"" + m_currentToken.inLine());
}

void	Parser::throw_DirectiveIsNotTerminated(const std::string& directive) const
{
	throw std::runtime_error("directive \"" + directive + "\" is not terminated by \";\"" + m_currentToken.inLine());
}

void	Parser::throw_InvalidNumberOfArguments(const std::string& directive) const
{
	throw std::runtime_error("invalid number of arguments in \"" + directive + "\" directive" + m_currentToken.inLine());
}

void	Parser::throw_InvalidValue(void) const
{
	throw std::runtime_error("invalid value \"" + m_currentToken.getTokenValue() + "\"" + m_currentToken.inLine());
}

void	Parser::throw_InvalidParameter(void) const
{
	throw std::runtime_error("invalid parameter \"" + m_currentToken.getTokenValue() + "\"" + m_currentToken.inLine());
}

void	Parser::throw_Unexpected(void) const
{
	throw std::runtime_error("unexpected \"" + m_currentToken.getTokenValue() + "\"" + m_currentToken.inLine());
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
