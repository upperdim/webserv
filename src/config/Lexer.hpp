
#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <cctype>
#include <vector>
#include <map>
#include "Token.hpp"
#include "Log.hpp"

class Lexer
{
public:
	Lexer(const std::string& _input);
	Lexer(const Lexer& other);
	~Lexer();
	
	void		tokenize(std::vector<Token>& tokens);
	void		printTokens(std::vector<Token>& tokens);

	bool		precededByComment = false;

private:
	void		markStart(void);
	char		peek(size_t offset = 0) const;
	char		advance(void);
	std::string	getCurrentLexeme() const;
	KeywordType	getKeywordType(const std::string& _keyword) const;
	void		skipWhitespaces(void);
	void		skipComment(void);

	bool		isMixedAlphanumeric() const;

	Token		nextToken(bool _precededByComment = false);
    Token		readNumber();
	Token		readAndClassify();

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
