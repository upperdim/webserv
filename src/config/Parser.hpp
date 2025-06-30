/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 11:02:35 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/30 11:53:08 by nmihaile         ###   ########.fr       */
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
#include "HTTP.hpp"
#include "Config.hpp"
#include "Token.hpp"
#include "Validator.hpp"
#include "Throw.hpp"
#include "Log.hpp"

class Parser
{
public:
	Parser(std::string configFilePath);
	~Parser();

	Config	parse(void);

private:
	std::vector<Token>	m_tokens;
	size_t				m_pos;

	typedef struct s_parsedDirectives {
		bool listen				= false;
		bool clientMaxBodySize	= false;
		bool root				= false;
		bool route				= false;
		bool autoIndex			= false;
		bool allowUpload		= false;
		bool uploadDir			= false;
	} t_parsedDirectives;

	const Token&	prev(void) const;
	const Token&	peek(void) const;
	const Token&	advance(void);
	bool			isAtEnd(void) const;
	bool			isValidKeyword(const Token& token) const;
	void			expect(TokenType _type, const Token& directive, const std::string& msg);
	void			expectNoArguments(void);

	void			skipEventsDirective(void);
	void			parseHttpDirective(Config& config);
	void			parseServerBlock(ServerBlock& server, t_parsedDirectives& parsedDirectives);
	void			parseServerDirectives(ServerBlock& server, t_parsedDirectives& parsedDirectives);
	void			parseLocationBlock(LocationBlock& location);
	void			parseLocationDirectives(LocationBlock& location);

	void			parseListenDirective(const Token& directive, std::vector<const Token*>& params, ServerBlock& server);
	void			parseServerNameDirective(const Token& directive, std::vector<const Token*>& params, ServerBlock& server);
	void			parseErrorPageDirective(const Token& directive, std::vector<const Token*>& params, ServerBlock& server);
	void			parseAllowMethodsDirective(const Token& directive, std::vector<const Token*>& params, LocationBlock& location);
	void			parseClientMaxBodySizeDirective(const Token& directive, std::vector<const Token*>& params, size_t& value);
	void			parseIndexDirective(const Token& directive, std::vector<const Token*>& params, std::string& index);
	void			parseUri(const Token& directive, std::vector<const Token*>& params, std::string& root);
	void			parseToggle(const Token& directive, std::vector<const Token*>& params, bool& autoIndex);
	void			parseExtension(const Token& directive, std::vector<const Token*>& params, std::string& ext);

	// rules and checks
	void			addDefaultLocationBlocks(std::vector<ServerBlock>& serverBlocks);

};

#endif