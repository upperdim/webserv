
#include "Token.hpp"

// we have to define the default value of this shared membervariable in the source file so that ot compiles
size_t	Token::m_maxLineDigits = 0;

Token::Token(const TokenType& _type, const KeywordType& _keywordType, const std::string& _value, size_t _line)
	:	type(_type),
		keywordType(_keywordType),
		value(_value),
		line(_line)
{
	m_maxLineDigits = countDigits(line);
}

Token::Token(const TokenType& _type, const std::string& _value, size_t _line)
	:	type(_type),
		keywordType(KeywordType::NONE),
		value(_value),
		line(_line)
{
	m_maxLineDigits = countDigits(line);
}

Token::Token(const TokenType& _type, size_t _line)
	:	type(_type),
		keywordType(KeywordType::NONE),
		value(""),
		line(_line)
{
	m_maxLineDigits = countDigits(line);
}

Token::Token(const Token& other)
	:	type(other.type),
		keywordType(other.keywordType),
		value(other.value),
		line(other.line)
{
	m_maxLineDigits = countDigits(line);
}

Token::~Token()
{
}


/* ************************************************************************** */
/* ************************************************************************** */


std::string	Token::toString(void) const
{
	std::string directive;
	if (type == TokenType::KEYWORD)
		directive = " " + tokenKeywordTypeToString();

	std::stringstream lineStr;
    lineStr << std::setw(m_maxLineDigits) << std::setfill('0') << line;

	std::string tokenStr = std::string(GREEN"[" LIGHTGREEN) + lineStr.str() + GREEN"] " LIGHTCYAN;
	tokenStr += tokenTypeToString() + LIGHTRED + directive;
	tokenStr += std::string(" ") + LIGHTYELLOW + value + DEFAULT;
	return (tokenStr);
}

std::string	Token::getTokenValue(void) const
{
	switch (type) {
		case (TokenType::KEYWORD):		return lowerCase(tokenKeywordTypeToString());
		case (TokenType::PARAM):		return value;
		case (TokenType::PATH):			return value;
		case (TokenType::URL):			return value;
		case (TokenType::NUMBER):		return value;
		case (TokenType::STRING):		return value;
		case (TokenType::COLON):		return value;
		case (TokenType::SEMICOLON):	return value;
		case (TokenType::OPEN_BRACE):	return value;
		case (TokenType::CLOSE_BRACE):	return value;
		case (TokenType::END_OF_INPUT):	return "end_of_input";
		case (TokenType::INVALID):		return std::string("invalid_token(") + value + ")";
		default:						return "unknown_token";
	}
}

std::string	Token::inLine(void) const
{
	return (std::string(" in line " LIGHTCYAN) + std::to_string(line) + DEFAULT);
}


/* ************************************************************************** */
/* ************************************************************************** */


std::string	Token::tokenTypeToString(void) const
{
	switch (type) {
		case TokenType::KEYWORD:		return "KEYWORD";
		case TokenType::PARAM:			return "PARAM";
		case TokenType::PATH:			return "PATH";
		case TokenType::URL:			return "URL";
		case TokenType::NUMBER: 		return "NUMBER";
		case TokenType::STRING: 		return "STRING";
		case TokenType::COLON:			return "COLON";
		case TokenType::SEMICOLON:		return "SEMICOLON";
		case TokenType::OPEN_BRACE:		return "OPEN_BRACE";
		case TokenType::CLOSE_BRACE:	return "CLOSE_BRACE";
		case TokenType::END_OF_INPUT:	return "END_OF_INPUT";
		case TokenType::INVALID:		return "INVALID";
		default:						return "UNKNOWN_TOKEN";
	}
}

std::string	Token::tokenKeywordTypeToString(void) const
{
	switch (keywordType) {
		case KeywordType::NONE:					return "NONE";
		case KeywordType::EVENTS:				return "EVENTS";
		case KeywordType::HTTP:					return "HTTP";
		case KeywordType::SERVER:				return "SERVER";
		case KeywordType::LOCATION: 			return "LOCATION";
		case KeywordType::LISTEN:				return "LISTEN";
		case KeywordType::SERVER_NAME:			return "SERVER_NAME";
		case KeywordType::ERROR_PAGE:			return "ERROR_PAGE";
		case KeywordType::CLIENT_MAX_BODY_SIZE:	return "CLIENT_MAX_BODY_SIZE";
		case KeywordType::ROOT:					return "ROOT";
		case KeywordType::ALIAS:				return "ALIAS";
		case KeywordType::INDEX:				return "INDEX";
		case KeywordType::ALLOW_METHODS:		return "ALLOW_METHODS";
		case KeywordType::RETURN:				return "RETURN";
		case KeywordType::AUTOINDEX:			return "AUTOINDEX";
		case KeywordType::CGI:					return "CGI";
		case KeywordType::ALLOW_UPLOAD:			return "ALLOW_UPLOAD";
		case KeywordType::UPLOAD_STORE:			return "UPLOAD_STORE";
		default: return "UNKNOWN_KEYWORD_TYPE";
	}
}

std::string Token::lowerCase(const std::string& str)
{
	std::string	lower;
	for (std::string::const_iterator it = str.cbegin(); it < str.cend(); ++it)
		lower += std::tolower(*it);
	return (lower);	
}

size_t	Token::countDigits(size_t nbr)
{
	if (nbr == 0)
		return 1;

	size_t count = 0;
	while (nbr != 0) {
		++count;
		nbr /= 10;
	}
	return count;
}


/* ************************************************************************** */
/* ************************************************************************** */


std::ostream&  operator << (std::ostream& os, const Token& token)
{
	os << token.toString();
	return (os);
}
