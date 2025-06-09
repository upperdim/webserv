/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 11:05:43 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/08 17:16:32 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <fstream>
#include <stdexcept>
#include <arpa/inet.h>	// for inet_pton()
#include <sys/types.h>	// for getaddrinfo()
#include <sys/socket.h>	// for getaddrinfo()
#include <netdb.h>		// for getaddrinfo()
#include "Config.hpp"
#include "Token.hpp"
#include "Lexer.hpp"
#include "Log.hpp"

class Parser
{
public:
	Parser(const Lexer& _lexer);
	~Parser();

	Config	parse(void);
	Config	mockParseConfig(std::string configFilePath);		// TODO: remove 
	
private:
	Parser(const Parser& other);
	Parser&	operator=(const Parser& rhs);

	ServerBlock		parseServer(void);
	void			parseListenDirective(ServerBlock& serverBlock);
	void			parsePortAfterHost(ServerBlock& serverBlock);
	
	void			consume(TokenType _type, std::string msg = "");
	void			ensureDirectiveTermination(const std::string& name);

	unsigned int	validatePort(const std::string& _port);

	std::string		readConfigFile(std::string configFilePath);		// TODO: remove 

	Lexer	m_lexer;
	Token	m_currentToken;
	Config	m_config;
};

#endif
