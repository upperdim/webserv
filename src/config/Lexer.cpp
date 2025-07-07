#include "Lexer.hpp"
#include <unordered_set>
#include "Utils.hpp"

Lexer::Lexer(const std::string& _input)
	:	m_input(_input),
		m_pos(0),
		m_line(1)
{
}

Lexer::Lexer(const Lexer& other)
	:	m_input(other.m_input),
		m_pos(0),
		m_line(other.m_line)
{
}

Lexer::~Lexer()
{
}


/* ************************************************************************** */
/* ************************************************************************** */


void	Lexer::tokenize(std::vector<Token>& tokens)
{
	while (m_pos < m_input.length()) {
		Token token = nextToken();

		//	TODO:	decide if we process INVALID tokens or if we just leave them out
		//			currently I decided to add INVALID tokens to the list… lets see…
		// if (token.type != TokenType::INVALID)
		tokens.emplace_back(token);
	}

	if (tokens.size() == 0 || tokens.back().type != TokenType::END_OF_INPUT)
		tokens.emplace_back(Token(TokenType::END_OF_INPUT, m_line));
}

Token	Lexer::nextToken(bool _precededByComment)
{
	precededByComment = _precededByComment;
	skipWhitespaces();

	if (m_pos >= m_input.length())
		return Token(TokenType::END_OF_INPUT, m_line);

	markStart();

	char c = peek();

	if (c == '#') {
		skipComment();
		skipWhitespaces();
		if (m_pos >= m_input.length())
			return Token(TokenType::END_OF_INPUT, m_line);
		return nextToken(true);
	}

	switch (c) {
		case ':':
			advance();
			return Token(TokenType::COLON, ":", m_line);
		case ';':
			advance();
			return Token(TokenType::SEMICOLON, ";", m_line);
		case '{':
			advance();
			return Token(TokenType::OPEN_BRACE, "{", m_line);
		case '}':
			advance();
			return Token(TokenType::CLOSE_BRACE, "}", m_line);
	}

	// Handle STRINGS
	if (c == '"' || c == '\'') {
		readString(c);
	}

	//	Handle numbers
	if (std::isdigit(c)) {
		if (isMixedAlphanumeric())
			return readAndClassify();	//	Domain/address starting with digit
		return readNumber();
	}

	//	Handle keywords and general parameters
	//	TODO:	I added '.' '-' and '_' is this ok
	if (std::isalpha(c) || c == '/' || c == '.' || c == '-' || c == '_') {
		return readAndClassify();
	}

	advance();
	return Token(TokenType::INVALID, std::string(1, c), m_line);
}

void	Lexer::printTokens(std::vector<Token>& tokens)
{
	for (const Token& token : tokens)
		std::cout << token << std::endl;
}


/* ************************************************************************** */
/* ************************************************************************** */


void	Lexer::markStart(void)
{
	m_startPos = m_pos;
}

char	Lexer::peek(size_t offset) const
{
	size_t idx = m_pos + offset;
	return  idx < m_input.length() ? m_input[idx] : '\0';
}

char	Lexer::advance(void)
{
	if (m_pos < m_input.length()) {
		char c = m_input[m_pos++];
		if (c == '\n')
			++m_line;
		return c;
	}
	return '\0';
}

std::string Lexer::getCurrentLexeme() const
{
	return m_input.substr(m_startPos, m_pos - m_startPos);
}

KeywordType	Lexer::getKeywordType(const std::string& _keyword) const
{
	if (directives.find(_keyword) != directives.end()) {
		return directives.at(_keyword);
	}
	return KeywordType::NONE;
}

void	Lexer::skipWhitespaces(void)
{
	while (m_pos < m_input.length() && std::isspace(peek()))
		advance();
}

void	Lexer::skipComment(void)
{
	precededByComment = true;
	advance();	//	consume '#'
	while (m_pos < m_input.length() && peek() != '\n')
		advance();
	if (peek() == '\n')
		advance();
}

bool	Lexer::isMixedAlphanumeric() const
{
	size_t	lookAhead = 0;

	// skip inital digits
	while (m_pos + lookAhead < m_input.length() && std::isdigit(peek(lookAhead)))
		++lookAhead;
	
	//	check if followed by IPv4 domain/address aka isalpha, '-', '.'
	if (m_pos + lookAhead < m_input.length()) {
		char nextChar = peek(lookAhead);
		return (std::isalpha(nextChar) || nextChar == '-' || nextChar == '.');
	}

	return false;
}

Token	Lexer::readString(char quote)
{
	advance();
	while (m_pos < m_input.length() && peek() != quote)
		advance();
	advance();
	std::string lexem = getCurrentLexeme();

	// 	//////////////////////////////////////////////////////////
	//	TODO:	add unquote here from branch 47-handle-post-method
	// Utils::unquote(lexem, quote)
	// 	//////////////////////////////////////////////////////////
	LOGT(Log::WARNING, "c=" << quote << " " << "|" << lexem << "|");
	// 	//////////////////////////////////////////////////////////

	return Token(TokenType::NUMBER, getCurrentLexeme(), m_line);
}

Token	Lexer::readNumber()
{
	while (m_pos < m_input.length() && (std::isdigit(peek()) || peek() == '.'))
		advance();
	return Token(TokenType::NUMBER, getCurrentLexeme(), m_line);
}

Token	Lexer::readAndClassify()
{
	while (m_pos < m_input.length() &&
			(std::isalnum(peek()) || peek() == '.' || peek() == '/' || peek() == '_' || peek() == '-')) {
		advance();
	}

	std::string lexeme = getCurrentLexeme();

	// check if it is a supported directive aka KEYWORD, PATH or URL
	KeywordType keywordType = getKeywordType(lexeme);
	if (Utils::startsWithHttp(lexeme) && 
		((peek() == ':' && peek(1) == '/' && peek(2) == '/') ||
		 (lexeme[4] == 's' && peek() == ':' && peek(1) == '/' && peek(2) == '/'))) {
		// advance all valid URL characrers
		// we are accepting queries '?' and fragments '#'
		// to support return directivs like:
		// return https://www.google.com/search?q=openai#top;
		// do we support: with user authority
		// https://user:pa$$w0rd@sub.domain-example.com:8080/path/to/resource?q=glsl+shading#section2
		// do we support: with ';'
		// https://example.com/products/item;id=42;color=blue?sort=price#details;

		static std::unordered_set<unsigned char> pathChars = {
			'-', '.', '_', '~', '!', '$', '&', '\'', '(', ')',
			'*', '+', ',', ';', '=', ':', '@', '/', '?', '#'
		};

		while (m_pos < m_input.length() && (std::isalnum(peek()) || pathChars.count(peek())) &&
		       !(peek() == ';' && std::isspace(peek(1)))) {
			advance();
		}
		lexeme = getCurrentLexeme();
	
		return Token(TokenType::URL, lexeme, m_line);
	} else if (keywordType != KeywordType::NONE) {
		return Token(TokenType::KEYWORD, keywordType, lexeme, m_line);
	} else if (lexeme[0] == '/') {
		return Token(TokenType::PATH, lexeme, m_line);
	}

	// By default we return a PARAM
	return Token(TokenType::PARAM, lexeme, m_line);
}
