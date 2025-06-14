/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser2.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 11:02:35 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/14 09:19:04 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>
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

	const Token&	peek(void) const;
	const Token&	advance(void);
	
};

#endif