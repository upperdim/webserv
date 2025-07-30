
#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>
#include <algorithm>		// std::find
#include <arpa/inet.h>		// inet_pton()
#include <sys/types.h>		// getaddrinfo()
#include <sys/socket.h>		// getaddrinfo()
#include <netdb.h>			// getaddrinfo()
#include "HTTP.hpp"
#include "Config.hpp"
#include "Token.hpp"
#include "Validator.hpp"
#include "Throw.hpp"

class Parser
{
public:
	Parser(std::string configFilePath, char *programName);

	Config	parse(void);

private:
	std::vector<Token>	m_tokens;
	size_t				m_pos;
	char*				m_programName;

	typedef struct s_parsedDirectives {
		bool events                 = false;
		bool http                   = false;
		bool listen					= false;
		bool clientMaxBodySize		= false;
		bool root					= false;
		std::vector<std::string>	locationRoutes;
		bool autoIndex				= false;
		bool allowUpload			= false;
		bool uploadStore			= false;
	} t_parsedDirectives;

	const Token&	prev(void) const;
	const Token&	peek(void) const;
	const Token&	advance(void);
	bool			isAtEnd(void) const;
	bool			isValidKeyword(const Token& token) const;
	void			expect(TokenType _type, const Token& directive, const std::string& msg);
	void			expectNoArguments(void);
	void			collectParameters(std::vector<const Token*>& params);

	void			skipEventsDirective(void);
	void			parseHttpDirective(Config& config);
	void			parseServerBlock(ServerBlock& server, t_parsedDirectives& parsedServerDirectives);
	void			parseServerDirectives(ServerBlock& server, t_parsedDirectives& parsedServerDirectives);
	void			parseLocationBlock(LocationBlock& location, t_parsedDirectives& parsedLocationDirectives);
	void			parseLocationDirectives(LocationBlock& location, t_parsedDirectives& parsedLocationDirectives);

	void			parseListenDirective(const Token& directive, std::vector<const Token*>& params, ServerBlock& server);
	void			parseServerNameDirective(const Token& directive, std::vector<const Token*>& params, ServerBlock& server);
	void			parseErrorPageDirective(const Token& directive, std::vector<const Token*>& params, std::map<int, std::string>& errorPagePaths);
	void			parseAllowMethodsDirective(const Token& directive, std::vector<const Token*>& params, LocationBlock& location);
	void			parseReturnDirective(const Token& directive, std::vector<const Token*>& params, std::string& target);
	void			parseClientMaxBodySizeDirective(const Token& directive, std::vector<const Token*>& params, size_t& value);
	void			parseIndexDirective(const Token& directive, std::vector<const Token*>& params, std::string& index);
	void			parsePath(const Token& directive, std::vector<const Token*>& params, std::string& path);
	void			parseToggle(const Token& directive, std::vector<const Token*>& params, bool& autoIndex);
	void			parseCgiExtension(const Token& directive, std::vector<const Token*>& params, LocationBlock& location);

	// rules and checks
	void			setFallbacks(Config& config);
	void			checksServerBlocksAndSetsDefaults(Config& config);

	// utils
	std::string		readFile(const std::string& filename);

};

#endif