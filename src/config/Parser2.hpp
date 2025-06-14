/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser2.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 11:02:35 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/14 13:01:45 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>
#include <arpa/inet.h>		// for inet_pton()
#include <sys/types.h>		// for getaddrinfo()
#include <sys/socket.h>		// for getaddrinfo()
#include <netdb.h>			// for getaddrinfo()
#include "colors.hpp"
#include "Config.hpp"
#include "Token.hpp"
#include "Validator.hpp"

class Parser
{
public:
	Parser(const std::vector<Token>& _tokens);
	~Parser();

	Config	parse(void);

private:
	const std::vector<Token>&	m_tokens;
	size_t						m_pos;

	const Token&	prev(void) const;
	const Token&	peek(void) const;
	const Token&	advance(void);
	bool			isAtEnd(void) const;
	bool			isValidKeyword(const Token& token) const;
	void			expect(TokenType _type, const std::string& msg);
	void			expectNoArguments(void);

	void			skipEventsDirective(void);
	void			parseHttpDirective(Config& config);
	void			parseServerBlock(ServerBlock& server);
	void			parseServerDirective(ServerBlock& server);
	void			parseListenDirective(const Token& directive, std::vector<const Token*>& params, ServerBlock& server);
	void			parseLocationBlock(LocationBlock& location);
	void			parseLocationDirective(LocationBlock& location);

	void			throw_SyntaxError(const std::string& msg) const;
	void			throw_Unexpected(const Token& token) const;
	void			throw_UnknownOrUnsupportedDirective(const Token& token) const;
	void			throw_InvalidNumberOfArguments(const Token& token) const;
	void			throw_InvalidIPAddr(const Token& token) const;
	void			throw_HostNotFound(const Token& token) const;
	void			throw_InvalidPort(const Token& directive, const Token& portToken) const;
	void			throw_InvalidParameter(const Token& token) const;

};

#endif