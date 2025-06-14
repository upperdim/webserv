/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser2.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 11:02:33 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/14 12:56:09 by nmihaile         ###   ########.fr       */
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
		if (!isValidKeyword(peek()))
			throw_UnknownOrUnsupportedDirective(peek());

		const Token& directive = peek();
		switch (directive.keywordType) {
			case KeywordType::EVENTS:
				skipEventsDirective();
				break ;
			case KeywordType::HTTP:
				parseHttpDirective(config);
				break ;
			// add any global supported directive here
			default:
				advance();	//	TODO: delete me later if not needed!!!!
				break ;
		}
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

bool	Parser::isValidKeyword(const Token& token) const
{
	return token.type == TokenType::KEYWORD && token.keywordType != KeywordType::NONE;
}

void	Parser::expect(TokenType _type, const std::string& msg)
{
	if (m_pos >= m_tokens.size() || m_tokens[m_pos].type != _type)
		throw_SyntaxError(msg);
	advance();
}

void	Parser::expectNoArguments(void)
{
	// throw if param is available
	if (peek().type == TokenType::PARAM ||
	    peek().type == TokenType::URI ||
	    peek().type == TokenType::NUMBER)
		throw_InvalidNumberOfArguments(prev());
}

void	Parser::skipEventsDirective(void)
{
	advance();	//	consumes EVENTS directive
	
	expectNoArguments();
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

void	Parser::parseHttpDirective(Config& config)
{
	advance();	//	consumes HTTP directive

	expectNoArguments();
	expect(TokenType::OPEN_BRACE, "expected \"{\"");

	while (m_pos < m_tokens.size() && !isAtEnd() && peek().type != TokenType::CLOSE_BRACE) {
		// expect KEYWORD
		if (!isValidKeyword(peek()))
			throw_UnknownOrUnsupportedDirective(peek());

		// select action based on KEYWORD_TYPE
		const Token& directive = peek();
		switch (directive.keywordType) {
			case KeywordType::SERVER: {
				ServerBlock serverBlock;
				parseServerBlock(serverBlock);
				config.serverBlocks.emplace_back(serverBlock);
				break ;
			}
			// add any Http scoped supported directive here
			default:
				throw_UnknownOrUnsupportedDirective(peek());
		}
	}
	expect(TokenType::CLOSE_BRACE, "expected \"}\"");
}

void	Parser::parseServerBlock(ServerBlock& server)
{
	advance();	//	consumes SERVER directive

	expectNoArguments();
	expect(TokenType::OPEN_BRACE, "expected \"{\"");

	while (m_pos < m_tokens.size() && !isAtEnd() && peek().type != TokenType::CLOSE_BRACE) {
		// expect KEYWORD
		if (!isValidKeyword(peek()))
			throw_UnknownOrUnsupportedDirective(peek());

		// select action based on KEYWORD_TYPE
		const Token& directive = peek();
		if (directive.keywordType == KeywordType::LOCATION) {
			LocationBlock location;
			parseLocationBlock(location);
			server.locationsBlocks.emplace_back(location);
		} else {
			parseServerDirective(server);
		}
	}
	expect(TokenType::CLOSE_BRACE, "expected \"}\"");
}

void	Parser::parseServerDirective(ServerBlock& server)
{
	const Token& directive = advance();	// grab the current directive token

	// grab all parameters
	std::vector<const Token*>	params;
	while (m_pos < m_tokens.size() && (
	       peek().type == TokenType::PARAM ||
	       peek().type == TokenType::URI ||
	       peek().type == TokenType::NUMBER ||
	       peek().type == TokenType::COLON)) {
		const Token& current = advance();
		params.emplace_back(&current);
	}
	expect(TokenType::SEMICOLON, "expected \";\"");

	if (directive.keywordType == KeywordType::LISTEN) {
		parseListenDirective(directive, params, server);
	} else {
			throw_UnknownOrUnsupportedDirective(directive);
	}
}

void	Parser::parseListenDirective(const Token& directive, std::vector<const Token*>& params, ServerBlock& server)
{
	if (params.size() == 0)
		throw_InvalidNumberOfArguments(directive);

	if (params[0]->type == TokenType::PARAM) {
		if (Validator::isIPAddr(params[0]->value)) {
			// we got an IP
			std::string ip = (params[0]->value == "localhost" ? "127.0.0.1" : params[0]->value);
			in_addr	addr;

			int result = inet_pton(AF_INET, ip.c_str(), &addr);
			if (result == 0)
				throw_InvalidIPAddr(*params[0]);
			server.host = addr.s_addr;
		} else if (Validator::isDomainName(params[0]->value)) {
			// we got a DomainName
			addrinfo hints;
			hints.ai_family = AF_INET;			// IPv4
			hints.ai_socktype = SOCK_STREAM;	// we want a: stream socket (typically used for TCP)
			addrinfo* res;						// result
			if (getaddrinfo(params[0]->value.c_str(), nullptr, &hints, &res) != 0)
				throw_HostNotFound(*params[0]);
			
			sockaddr_in* ipv4 = reinterpret_cast<sockaddr_in*>(res->ai_addr);
			server.host = ipv4->sin_addr.s_addr;
			freeaddrinfo(res);
		} else 
			throw_InvalidPort(directive, *params[0]);

		if (params.size() > 1 && params[1]->type == TokenType::COLON)
		{
			//	we got ":" so we have to check for a give PORT
			if (params.size() > 2 && params[2]->type != TokenType::NUMBER)
				throw_InvalidPort(directive, *params[2]);
			
			unsigned int port;
			if (!Validator::isValidPort(params[2]->value, port))
				throw_InvalidPort(directive, *params[2]);
			server.listenPort = port;
		}
		else {
			if (params.size() > 1)
				throw_InvalidParameter(*params[1]);
			server.listenPort = 80;
		}
	} else if (params[0]->type == TokenType::NUMBER) {
		// we got a PORT
		if (params.size() > 1)
			throw_InvalidNumberOfArguments(directive);

		server.host = INADDR_ANY;

		unsigned int port;
		if (!Validator::isValidPort(params[0]->value, port))
			throw_InvalidPort(directive, *params[0]);
		server.listenPort = port;
	}
}

void	Parser::parseLocationBlock(LocationBlock& location)
{
	advance();	//	consumes LOCATION directive

	//	expect PARAMS
	//	TODO: the rest …
	(void)location;

}

void	Parser::parseLocationDirective(LocationBlock& location)
{
	(void)location;
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

void	Parser::throw_InvalidIPAddr(const Token& token) const
{
	throw std::runtime_error("invalid IP address \"" + token.getTokenValue() + "\"" + token.inLine());
}

void	Parser::throw_HostNotFound(const Token& token) const
{
	throw std::runtime_error("host not found in \"" + token.getTokenValue() + "\"" + token.inLine());
}

void	Parser::throw_InvalidPort(const Token& directive, const Token& portToken) const
{
	throw std::runtime_error("invalid port in \"" + portToken.getTokenValue() + "\" of the \"" + directive.getTokenValue() + "\" directive" + portToken.inLine());
}

void	Parser::throw_InvalidParameter(const Token& token) const
{
	throw std::runtime_error("invalid parameter \"" + token.getTokenValue() + "\"" + token.inLine());
}
