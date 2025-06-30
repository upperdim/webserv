/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Throw.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/16 09:07:32 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/30 12:54:02 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Throw.hpp"

Throw::~Throw()
{
}


/* ************************************************************************** */
/* ************************************************************************** */


void	Throw::SyntaxError(size_t pos, const std::vector<Token>& tokens, const std::string& msg)
{
	throw std::runtime_error("Syntax error" + (pos < tokens.size() ? tokens[pos].inLine() : tokens.back().inLine()) + RED + (msg.empty() ? "" : ": " + msg));
}

void	Throw::Unexpected(const Token& token)
{
	throw std::runtime_error("unexpected \"" + token.getTokenValue() + "\"" + token.inLine());
}

void	Throw::UnknownOrUnsupportedDirective(const Token& token)
{
	throw std::runtime_error("unknown or unsupported directive \"" + token.getTokenValue() + "\"" + token.inLine());
}

void	Throw::InvalidNumberOfArguments(const Token& token)
{
	throw std::runtime_error("invalid number of arguments in \"" + token.getTokenValue() + "\" directive" + token.inLine());
}

void	Throw::InvalidIPAddr(const Token& token)
{
	throw std::runtime_error("invalid IP address \"" + token.getTokenValue() + "\"" + token.inLine());
}

void	Throw::HostNotFound(const Token& token)
{
	throw std::runtime_error("host not found in \"" + token.getTokenValue() + "\"" + token.inLine());
}

void	Throw::NoHost(const Token& directive, const Token& token)
{
	throw std::runtime_error("no host in \"" + token.getTokenValue() + "\" of the \"" + directive.getTokenValue() + "\" directive" + token.inLine());
}

void	Throw::InavlidHost(const Token& directive, const Token& token)
{
	throw std::runtime_error("invalid host in \"" + token.getTokenValue() + "\" of the \"" + directive.getTokenValue() + "\" directive" + token.inLine());
}

void	Throw::InvalidPort(const Token& directive, const Token& portToken)
{
	throw std::runtime_error("invalid port in \"" + portToken.getTokenValue() + "\" of the \"" + directive.getTokenValue() + "\" directive" + portToken.inLine());
}

void	Throw::InvalidParameter(const Token& token)
{
	throw std::runtime_error("invalid parameter \"" + token.getTokenValue() + "\"" + token.inLine());
}

void	Throw::AccpetsOnlyDomainNames(const Token& token)
{
	throw std::runtime_error("accpets only \"domain names\" as server_name \"" + token.getTokenValue() + "\"" + token.inLine());
}

void	Throw::InvalidValue(const Token& token)
{
	throw std::runtime_error("invalid value \"" + token.getTokenValue() + "\"" + token.inLine());
}

void	Throw::InvalidErrorpageNbr(const Token& token)
{
	throw std::runtime_error("value \"" + token.getTokenValue() + "\" must be between 300 and 599" + token.inLine());
}

void	Throw::DirectiveIsNotTerminated(const Token& token)
{
	throw std::runtime_error("directive \"" + token.getTokenValue() + "\" is not terminated by \";\"" + token.inLine());
}

void	Throw::InvalidReturnCode(const Token& token)
{
	throw std::runtime_error("invalid return code \"" + token.getTokenValue() + "\"" + token.inLine());
}

void	Throw::InvalidExtension(const Token& token)
{
	throw std::runtime_error("invalid extension \"" + token.getTokenValue() + "\"" + token.inLine());
}

void	Throw::FailedToConvertDomainToIP(const Token& token)
{
	throw std::runtime_error("Failed to convert \"" + token.getTokenValue() + "\" to IP address" + token.inLine());
}

void	Throw::DuplicateDirective(const Token& directive)
{
	throw std::runtime_error("\"" + directive.getTokenValue() + "\" directive is duplicate" + directive.inLine());
}

void	Throw::DuplicateListenDirective(const Token& token, const ServerBlock& server)
{
	throw std::runtime_error("a duplicate \"listen\" " + server.listenHostStr + ":" + std::to_string(server.listenPort) + token.inLine());
}

void	Throw::DuplicateLocationDirective(const Token& token, std::string& route)
{
	throw std::runtime_error("duplicate location \"" + route + "\"" + token.inLine());
}
