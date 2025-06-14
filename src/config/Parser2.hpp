/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser2.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 11:02:35 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/14 11:14:27 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>
#include "colors.hpp"
#include "Config.hpp"
#include "Token.hpp"

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
	void			parseServerDirective(ServerBlock& serverBlock);

	void			throw_SyntaxError(const std::string& msg) const;
	void			throw_Unexpected(const Token& token) const;
	void			throw_UnknownOrUnsupportedDirective(const Token& token) const;
	void			throw_InvalidNumberOfArguments(const Token& token) const;

};

#endif