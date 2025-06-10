/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 19:11:45 by tunsal            #+#    #+#             */
/*   Updated: 2025/06/10 17:46:09 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <cctype>
#include <map>
#include "Token.hpp"
#include "Log.hpp"

class Lexer
{
public:
	Lexer(const std::string& _input);
	Lexer(const Lexer& other);
	~Lexer();
	
	Token	nextToken(void);

	bool	precededByComment = false;

private:
	void	markStart(void);
	char	peek(size_t offset = 0) const;
	char	advance(void);
	void	skipWhitespaces(void);
	void	skipComment(void);
	
	size_t	isIPAddress(size_t start) const;	// TODO: delete me

	std::string	m_input;
	size_t		m_pos;
	size_t		m_startPos;
	size_t		m_line;

    std::map<std::string, KeywordType> directives = {
		{"events",					KeywordType::EVENTS},
		{"http",					KeywordType::HTTP},
		{"server",					KeywordType::SERVER},
		{"location",				KeywordType::LOCATION},

		{"listen", 					KeywordType::LISTEN},
		{"server_name", 			KeywordType::SERVER_NAME},
		{"error_page", 				KeywordType::ERROR_PAGE},
		{"client_max_body_size",	KeywordType::CLIENT_MAX_BODY_SIZE},
		{"root", 					KeywordType::ROOT},
		{"index", 					KeywordType::INDEX},

		{"allow_methods",			KeywordType::ALLOW_METHODS },
		{"return",					KeywordType::RETURN },
		{"autoindex",				KeywordType::AUTOINDEX },
		{"cgi_extension",			KeywordType::CGI_EXTENSION },
		{"allow_upload",			KeywordType::ALLOW_UPLOAD },
		{"upload_store",			KeywordType::UPLOAD_STORE }
	};
};

#endif
