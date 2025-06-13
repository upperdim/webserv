/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser2.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 11:02:35 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/13 11:11:45 by nmihaile         ###   ########.fr       */
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
	Config						m_config = {};
};

#endif