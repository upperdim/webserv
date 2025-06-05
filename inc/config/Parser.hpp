/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 11:05:43 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/05 18:01:19 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <fstream>
#include <stdexcept>
#include "Token.hpp"
#include "Lexer.hpp"
#include "Log.hpp"

class Parser
{
public:
	Parser(const Lexer& _lexer);
	~Parser();

	std::string	parse();	// TODO: return config struct

private:
	Parser(const Parser& other);
	Parser&	operator=(const Parser& rhs);

	Lexer			m_lexer;
	// Config			m_config;
};

#endif