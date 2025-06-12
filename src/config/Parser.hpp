/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 11:05:43 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/12 17:26:21 by nmihaile         ###   ########.fr       */
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
#include "Validator.hpp"
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

	enum class ThrowType {
		UNEXPECTED,
		INVALID_NUMBER_OF_ARGUMENTS,
		INVALID_PARAMETER
	};

	void			parseEvents(void);
	void			parseHttp(void);
	ServerBlock		parseServer(void);
	void			parseListenDirective(ServerBlock& serverBlock);
	void			parseServerNameDirective(ServerBlock& serverBlock);
	void			parseErrorPageDirective(ServerBlock& serverBlock);
	void			parseClientMaxBodySize(size_t& _clientMaxBodySize);

	void			consume(TokenType _type, ThrowType _throwType, std::string directive = "");
	void			ensureDirectiveTermination(const std::string& name);

	void			throw_DirectiveIsNotAllowed(const std::string& directive) const;
	void			throw_UnknownOrUnsupportedDirective(const std::string& directive) const;
	void			throw_UnexpectedEndOfFile(const std::string& expected, const std::string& directive) const;
	void			throw_DirectiveIsDuplicate(const std::string& directive) const;
	void			throw_InvalidIPAddr(void) const;
	void			throw_HostNotFound(void) const;
	void			throw_InvalidPort(const std::string& directive) const;
	void			throw_AccpetsOnlyDomainNames(void) const;
	void			throw_DirectiveIsNotTerminated(const std::string& directive) const;
	void			throw_InvalidNumberOfArguments(const std::string& directive) const;
	void			throw_InvalidValue(void) const;
	void			throw_InvalidParameter(void) const;
	void			throw_Unexpected(void) const;
	void			throw_FailedToConvertStringToNumber(void) const;

	std::string		readConfigFile(std::string configFilePath);		// TODO: remove 

	Lexer	m_lexer;
	Token	m_currentToken;
	Config	m_config;
};

#endif
