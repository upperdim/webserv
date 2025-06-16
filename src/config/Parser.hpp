/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 11:02:35 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/16 13:44:45 by nmihaile         ###   ########.fr       */
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
#include <algorithm>		// for std::find in parseAllowMethodsDirective() method
#include "colors.hpp"
#include "Http.hpp"
#include "Config.hpp"
#include "Token.hpp"
#include "Validator.hpp"
#include "Throw.hpp"
#include "Log.hpp"

class Parser
{
public:
	Parser(const std::vector<Token>& _tokens);
	~Parser();

	Config	parse(void);

	Config		mockParseConfig(std::string configFilePath);	//	TODO:	remove 
	std::string	readConfigFile(std::string configFilePath);		//	TODO:	remove 

private:
	const std::vector<Token>&	m_tokens;
	size_t						m_pos;

	const Token&	prev(void) const;
	const Token&	peek(void) const;
	const Token&	advance(void);
	bool			isAtEnd(void) const;
	bool			isValidKeyword(const Token& token) const;
	void			expect(TokenType _type, const Token& directive, const std::string& msg);
	void			expectNoArguments(void);

	void			skipEventsDirective(void);
	void			parseHttpDirective(Config& config);
	void			parseServerBlock(ServerBlock& server);
	void			parseServerDirective(ServerBlock& server);
	void			parseLocationBlock(LocationBlock& location);
	void			parseLocationDirective(LocationBlock& location);

	void			parseListenDirective(const Token& directive, std::vector<const Token*>& params, ServerBlock& server);
	void			parseServerNameDirective(const Token& directive, std::vector<const Token*>& params, ServerBlock& server);
	void			parseErrorPageDirective(const Token& directive, std::vector<const Token*>& params, ServerBlock& server);
	void			parseAllowMethodsDirective(const Token& directive, std::vector<const Token*>& params, LocationBlock& location);
	void			parseClientMaxBodySizeDirective(const Token& directive, std::vector<const Token*>& params, size_t& value);
	void			parseIndexDirective(const Token& directive, std::vector<const Token*>& params, std::string& index);
	void			parseReturnDirective(const Token& directive, std::vector<const Token*>& params, ReturnRoute& returnRoute);
	void			parseUriDirective(const Token& directive, std::vector<const Token*>& params, std::string& root);
	void			parseToggleDirective(const Token& directive, std::vector<const Token*>& params, bool& autoIndex);
	void			parseExtensionDirective(const Token& directive, std::vector<const Token*>& params, std::string& ext);

};

#endif