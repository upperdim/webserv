/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Throw.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/16 09:07:33 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/16 09:16:20 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef THROW_HPP
#define THROW_HPP

#include <string>
#include <vector>
#include <stdexcept>
#include "colors.hpp"
#include "Token.hpp"

class Throw
{
public:
	~Throw();

	static void	SyntaxError(size_t pos, const std::vector<Token>& tokens, const std::string& msg);
	static void	Unexpected(const Token& token);
	static void	UnknownOrUnsupportedDirective(const Token& token);
	static void	InvalidNumberOfArguments(const Token& token);
	static void	InvalidIPAddr(const Token& token);
	static void	HostNotFound(const Token& token);
	static void	NoHost(const Token& directive, const Token& token);
	static void	InavlidHost(const Token& directive, const Token& token);
	static void	InvalidPort(const Token& directive, const Token& portToken);
	static void	InvalidParameter(const Token& token);
	static void	AccpetsOnlyDomainNames(const Token& token);
	static void	InvalidValue(const Token& token);
	static void	InvalidErrorpageNbr(const Token& token);
	static void	DirectiveIsNotTerminated(const Token& token);
	static void	InvalidReturnCode(const Token& token);

private:
	Throw();
};

#endif
