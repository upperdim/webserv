/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser2.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 11:02:33 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/15 12:05:42 by nmihaile         ###   ########.fr       */
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

	if (m_tokens.size() == 0 || isAtEnd()) {
		//	TODO:	return a valid default config
		return config;
	}

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
				throw_UnknownOrUnsupportedDirective(peek());
				// advance();	//	TODO: delete me later if not needed!!!!
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

void	Parser::expect(TokenType _type, const Token& directive, const std::string& msg)
{
	if (_type == TokenType::SEMICOLON && !isAtEnd()) {
	    if (peek().type == TokenType::CLOSE_BRACE) {
			throw_Unexpected(peek());
		} else if (peek().type == TokenType::OPEN_BRACE) {
			throw_DirectiveIsNotTerminated(directive);
		}
	}

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
	const Token& directive = advance();	//	consumes EVENTS directive
	
	expectNoArguments();
	expect(TokenType::OPEN_BRACE, directive, "expected \"{\"");

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
	expect(TokenType::CLOSE_BRACE, directive, "expected \"}\"");
}

void	Parser::parseHttpDirective(Config& config)
{
	const Token& directive = advance();	//	consumes HTTP directive

	expectNoArguments();
	expect(TokenType::OPEN_BRACE, directive, "expected \"{\"");

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
	expect(TokenType::CLOSE_BRACE, directive, "expected \"}\"");
}

void	Parser::parseServerBlock(ServerBlock& server)
{
	const Token& directive = advance();	//	consumes SERVER directive

	expectNoArguments();
	expect(TokenType::OPEN_BRACE, directive, "expected \"{\"");

	while (m_pos < m_tokens.size() && !isAtEnd() && peek().type != TokenType::CLOSE_BRACE) {
		// expect KEYWORD
		if (!isValidKeyword(peek()))
			throw_UnknownOrUnsupportedDirective(peek());

		// select action based on KEYWORD_TYPE
		const Token& directive = peek();
		if (directive.keywordType == KeywordType::LOCATION) {
			LocationBlock location;
			parseLocationBlock(location);
			server.locationBlocks.emplace_back(location);
		} else {
			parseServerDirective(server);
		}
	}
	expect(TokenType::CLOSE_BRACE, directive, "expected \"}\"");
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
	       peek().type == TokenType::COLON ||
	       peek().type == TokenType::INVALID)) {
		const Token& current = advance();
		params.emplace_back(&current);
	}
	expect(TokenType::SEMICOLON, directive,  "expected \";\"");

	if (directive.keywordType == KeywordType::LISTEN) {
		parseListenDirective(directive, params, server);
	} else if (directive.keywordType == KeywordType::SERVER_NAME) {
		parseServerNameDirective(directive, params, server);
	} else if (directive.keywordType == KeywordType::ERROR_PAGE) {
		parseErrorPageDirective(directive, params, server);
	} else if (directive.keywordType == KeywordType::CLIENT_MAX_BODY_SIZE) {
		parseClientMaxBodySizeDirective(directive, params, server.clientMaxBodySize);
	} else if (directive.keywordType == KeywordType::ROOT) {
		parseRootDirective(directive, params, server.root);
	} else if (directive.keywordType == KeywordType::INDEX) {
		parseIndexDirective(directive, params, server.index);
	} else {
		throw_UnknownOrUnsupportedDirective(directive);
	}
}

void	Parser::parseListenDirective(const Token& directive, std::vector<const Token*>& params, ServerBlock& server)
{
	if (params.size() == 0)
		throw_InvalidNumberOfArguments(directive);
	if (params[0]->type == TokenType::INVALID)
		throw_HostNotFound(directive);
	if (params[0]->type == TokenType::URI)
		throw_InavlidHost(directive, *params[0]);
	if (params[0]->type == TokenType::COLON) {
		if (params.size() > 1 && params[1]->type == TokenType::NUMBER) {
			const Token combinedToken(TokenType::PARAM, std::string(params[0]->value) + params[1]->value, params[0]->line);
			throw_NoHost(directive, combinedToken);
		}
		throw_InvalidPort(directive, *params[0]);
	}

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
			if (params.size() > 2) {
				if (params[2]->type != TokenType::NUMBER)
					throw_InvalidPort(directive, *params[2]);
				
				unsigned int port;
				if (!Validator::isValidPort(params[2]->value, port))
					throw_InvalidPort(directive, *params[2]);
				server.listenPort = port;
			} else {
				const Token combinedToken(TokenType::PARAM, std::string(params[0]->value) + params[1]->value, params[0]->line);
				throw_InvalidPort(directive, combinedToken);
			}
		}
		else {
			if (params.size() > 1)
				throw_InvalidParameter(*params[1]);
			server.listenPort = 80;
		}
	} else if (params[0]->type == TokenType::NUMBER) {
		// we got a PORT
		if (params.size() > 1) {
			//	TODO: clean up the comments below, they are an old condition for a different throw method
			// if (params[1]->type == TokenType::URI)
				throw_InvalidParameter(*params[1]);
			// throw_InvalidNumberOfArguments(directive);
		}

		server.host = INADDR_ANY;

		unsigned int port;
		if (!Validator::isValidPort(params[0]->value, port))
			throw_InvalidPort(directive, *params[0]);
		server.listenPort = port;
	}
}

void	Parser::parseServerNameDirective(const Token& directive, std::vector<const Token*>& params, ServerBlock& server)
{
	if (params.size() == 0)
		throw_InvalidNumberOfArguments(directive);

	for (size_t i = 0; i < params.size(); ++i) {
		if (params[i]->type == TokenType::PARAM && Validator::isValidServerName(params[i]->value))
			server.serverNames.emplace_back(params[i]->value);
		else
			throw_AccpetsOnlyDomainNames(*params[i]);
	}
}

void	Parser::parseErrorPageDirective(const Token& directive, std::vector<const Token*>& params, ServerBlock& server)
{
	if (params.size() != 2)
		throw_InvalidNumberOfArguments(directive);

	if (params[0]->type != TokenType::NUMBER)
		throw_InvalidValue(*params[0]);
	if (!(params[1]->type != TokenType::PARAM ||
	      params[1]->type != TokenType::URI ||
	      params[1]->type != TokenType::NUMBER))
		throw_InvalidValue(*params[1]);

	int error_nbr = 0;
	if (!Validator::isValidErrorPageNbr(params[0]->value, error_nbr))
		throw_InvalidErrorpageNbr(*params[0]);

	server.errorPagePaths[error_nbr] = params[1]->value;
}

void	Parser::parseLocationBlock(LocationBlock& location)
{
	const Token& directive = advance();	//	consumes LOCATION directive

	// grab all parameters
	std::vector<const Token*>	params;
	while (m_pos < m_tokens.size() && (
	       peek().type == TokenType::PARAM ||
	       peek().type == TokenType::URI ||
	       peek().type == TokenType::NUMBER ||
	       peek().type == TokenType::COLON ||
	       peek().type == TokenType::INVALID)) {
		const Token& current = advance();
		params.emplace_back(&current);
	}
	expect(TokenType::OPEN_BRACE, directive, "expected \"{\"");

	//	validate PARAMS
	if (params.size() != 1)
		throw_InvalidNumberOfArguments(directive);
	if (params[0]->type != TokenType::URI)
		throw_InvalidValue(directive);

	//	TODO:	do we have to validate this route here???
	location.route = params[0]->value;

	while (m_pos < m_tokens.size() && !isAtEnd() && peek().type != TokenType::CLOSE_BRACE) {
		// expect KEYWORD
		if (!isValidKeyword(peek()))
			throw_UnknownOrUnsupportedDirective(peek());
		parseLocationDirective(location);
	}
	expect(TokenType::CLOSE_BRACE, directive, "expected \"}\"");
}

void	Parser::parseLocationDirective(LocationBlock& location)
{
	const Token& directive = advance();	// grab the current directive token

	// grab all parameters
	std::vector<const Token*>	params;
	while (m_pos < m_tokens.size() && (
	       peek().type == TokenType::PARAM ||
	       peek().type == TokenType::URI ||
	       peek().type == TokenType::NUMBER ||
	       peek().type == TokenType::COLON ||
	       peek().type == TokenType::INVALID)) {
		const Token& current = advance();
		params.emplace_back(&current);
	}
	expect(TokenType::SEMICOLON, directive,  "expected \";\"");

	// if (directive.keywordType == KeywordType::SERVER_NAME) {
	// 	parseServerNameDirective(directive, params, server);
	// } else if (directive.keywordType == KeywordType::ERROR_PAGE) {
	// 	parseErrorPageDirective(directive, params, server);
	if (directive.keywordType == KeywordType::CLIENT_MAX_BODY_SIZE) {
		parseClientMaxBodySizeDirective(directive, params, location.clientMaxBodySize);
	} else if (directive.keywordType == KeywordType::ROOT) {
		parseRootDirective(directive, params, location.root);
	} else if (directive.keywordType == KeywordType::INDEX) {
		parseIndexDirective(directive, params, location.index);
	} else {
		throw_UnknownOrUnsupportedDirective(directive);
	}
}

//	multiscope directive method expects the clientMaxBodySize value, so it can be set in the different scopes
void	Parser::parseClientMaxBodySizeDirective(const Token& directive, std::vector<const Token*>& params, size_t& value)
{
	if (params.size() != 1)
		throw_InvalidNumberOfArguments(directive);
	if (params[0]->value.find('.') != std::string::npos)
		throw_InvalidValue(*params[0]);

	size_t bodySizeValue;
	try {
		bodySizeValue = std::stoul(params[0]->value);
		if (params[0]->value.back() == 'k' || params[0]->value.back() == 'K')
			bodySizeValue *= 1024;
		else if (params[0]->value.back() == 'm' || params[0]->value.back() == 'M')
			bodySizeValue *= 1024 * 1024;
		else if (params[0]->value.back() == 'g' || params[0]->value.back() == 'G')
			bodySizeValue *= 1024 * 1024 * 1024;
	} catch(...) {
		throw_InvalidValue(*params[0]);
	}
	value = bodySizeValue;
}

//	multiscope directive method expects the root string, so it can be set in the different scopes
void	Parser::parseRootDirective(const Token& directive, std::vector<const Token*>& params, std::string& root)
{
	if (params.size() != 1)
		throw_InvalidNumberOfArguments(directive);
	if (params[0]->type != TokenType::URI)
		throw_InvalidValue(*params[0]);

	//	TODO:	we currently accept any URI
	//			IS this good enough for use ??
	root = params[0]->value;
}

//	multiscope directive method expects the index string, so it can be set in the different scopes
void	Parser::parseIndexDirective(const Token& directive, std::vector<const Token*>& params, std::string& index)
{
	//	TODO:	we currently go with single param first, since subject doesn't specifiy it as plural, see Notion
	if (params.size() != 1)
		throw_InvalidNumberOfArguments(directive);
	if (!(params[0]->type == TokenType::PARAM ||
		  params[0]->type == TokenType::URI ||
		  params[0]->type == TokenType::NUMBER))
		throw_InvalidValue(*params[0]);

	//	TODO:	we currently accept only one parameter
	//			and we accept any PARAM, URI or NUMBER
	index = params[0]->value;
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

void	Parser::throw_NoHost(const Token& directive, const Token& token) const
{
	throw std::runtime_error("no host in \"" + token.getTokenValue() + "\" of the \"" + directive.getTokenValue() + "\" directive" + token.inLine());
}

void	Parser::throw_InavlidHost(const Token& directive, const Token& token) const
{
	throw std::runtime_error("invalid host in \"" + token.getTokenValue() + "\" of the \"" + directive.getTokenValue() + "\" directive" + token.inLine());
}

void	Parser::throw_InvalidPort(const Token& directive, const Token& portToken) const
{
	throw std::runtime_error("invalid port in \"" + portToken.getTokenValue() + "\" of the \"" + directive.getTokenValue() + "\" directive" + portToken.inLine());
}

void	Parser::throw_InvalidParameter(const Token& token) const
{
	throw std::runtime_error("invalid parameter \"" + token.getTokenValue() + "\"" + token.inLine());
}

void	Parser::throw_AccpetsOnlyDomainNames(const Token& token) const
{
	throw std::runtime_error("accpets only \"domain names\" as server_name \"" + token.getTokenValue() + "\"" + token.inLine());
}

void	Parser::throw_InvalidValue(const Token& token) const
{
	throw std::runtime_error("invalid value \"" + token.getTokenValue() + "\"" + token.inLine());
}

void	Parser::throw_InvalidErrorpageNbr(const Token& token) const
{
	throw std::runtime_error("value \"" + token.getTokenValue() + "\" must be between 300 and 599" + token.inLine());
}

void	Parser::throw_DirectiveIsNotTerminated(const Token& token) const
{
	throw std::runtime_error("directive \"" + token.getTokenValue() + "\" is not terminated by \";\"" + token.inLine());
}
