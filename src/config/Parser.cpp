
#include "Parser.hpp"
#include <filesystem>
#include <algorithm>
#include <limits>
#include "Lexer.hpp"
#include "webserv.hpp"
#include "Log.hpp"

Parser::Parser(std::string configFilePath, char *programName)
	:	m_pos(0),
		m_programName(programName)
{
	Lexer lexer(readFile(configFilePath));
	lexer.tokenize(m_tokens);
}


/* ************************************************************************** */
/* ************************************************************************** */


Config	Parser::parse(void)
{
	Config config = {};
	setFallbacks(config);

	if (m_tokens.size() == 0 || isAtEnd()) {
		//	TODO:	return a valid default config
		//			set default value for allow_methods					
		return config;
	}

	t_parsedDirectives parsedRootDirectives;

	while (m_pos < m_tokens.size() && !isAtEnd()) {
		// expecting a KEYWORD or throw accordingly
		if (peek().type == TokenType::SEMICOLON ||
			peek().type == TokenType::OPEN_BRACE ||
			peek().type == TokenType::CLOSE_BRACE)
			Throw::Unexpected(peek());
		if (!isValidKeyword(peek()))
			Throw::UnknownOrUnsupportedDirective(peek());

		const Token& directive = peek();
		switch (directive.keywordType) {
			case KeywordType::EVENTS:
				if (parsedRootDirectives.events)
					Throw::DuplicateDirective(directive);
				parsedRootDirectives.events = true;
				skipEventsDirective();
				break;
			case KeywordType::HTTP:
				if (parsedRootDirectives.http)
					Throw::DuplicateDirective(directive);
				parsedRootDirectives.http = true;
				parseHttpDirective(config);
				break;
			// add any global supported directive here
			default:
				Throw::UnknownOrUnsupportedDirective(peek());
		}
	}

	checksServerBlocksAndSetsdefaults(config);

	return config;
}


/* ************************************************************************** */
/* ************************************************************************** */


const Token&	Parser::prev(void) const
{
	if (m_pos >= 1)
		return m_tokens[m_pos - 1];
	return m_tokens[0];
}

const Token&	Parser::peek(void) const
{
	return m_tokens[m_pos];
}

const Token&	Parser::advance(void)
{
	return m_tokens[m_pos++];
}

bool	Parser::isAtEnd(void) const
{
	return m_pos < m_tokens.size() && m_tokens[m_pos].type == TokenType::END_OF_INPUT;
}

bool	Parser::isValidKeyword(const Token& token) const
{
	return token.type == TokenType::KEYWORD && token.keywordType != KeywordType::NONE;
}

void	Parser::expect(TokenType _type, const Token& directive, const std::string& msg)
{
	if (_type == TokenType::SEMICOLON && !isAtEnd()) {
	    if (peek().type == TokenType::CLOSE_BRACE) {
			Throw::Unexpected(peek());
		} else if (peek().type == TokenType::OPEN_BRACE) {
			Throw::DirectiveIsNotTerminated(directive);
		}
	}

	if (m_pos >= m_tokens.size() || m_tokens[m_pos].type != _type)
		Throw::SyntaxError(m_pos, m_tokens, msg);
	advance();
}

void	Parser::expectNoArguments(void)
{
	// throw if param is available
	if (peek().type == TokenType::PARAM ||
	    peek().type == TokenType::URI ||
	    peek().type == TokenType::NUMBER ||
	    peek().type == TokenType::INVALID)
		Throw::InvalidNumberOfArguments(prev());
}

void	Parser::skipEventsDirective(void)
{
	const Token& directive = advance();	//	consumes EVENTS directive
	
	expectNoArguments();
	expect(TokenType::OPEN_BRACE, directive, "expected \"{\"");

	// count scopes and skips any tokens inside EVENTS directive
	size_t braceCount = 1;
	while (m_pos < m_tokens.size() && !isAtEnd()) {
		if (peek().type == TokenType::OPEN_BRACE)
			++braceCount;
		if (peek().type == TokenType::CLOSE_BRACE)
		{
			if (braceCount == 1)
				break ;
			--braceCount;
		}
		advance();
	}
	expect(TokenType::CLOSE_BRACE, directive, "expected \"}\"");
}

void	Parser::parseHttpDirective(Config& config)
{
	const Token& directive = advance();	//	consumes HTTP directive

	expectNoArguments();
	expect(TokenType::OPEN_BRACE, directive, "expected \"{\"");

	while (m_pos < m_tokens.size() && !isAtEnd() && peek().type != TokenType::CLOSE_BRACE) {
		// expect KEYWORD
		if (!isValidKeyword(peek()))
			Throw::UnknownOrUnsupportedDirective(peek());

		// select action based on KEYWORD_TYPE
		const Token& directive = peek();
		switch (directive.keywordType) {
			case KeywordType::SERVER: {
				ServerBlock serverBlock;
				t_parsedDirectives parsedServerDirectives;
				parseServerBlock(serverBlock, parsedServerDirectives);
				config.serverBlocks.emplace_back(serverBlock);
				break ;
			}
			// add any Http scoped supported directive here
			default:
				Throw::UnknownOrUnsupportedDirective(peek());
		}
	}
	expect(TokenType::CLOSE_BRACE, directive, "expected \"}\"");
}

void	Parser::parseServerBlock(ServerBlock& server, t_parsedDirectives& parsedServerDirectives)
{
	const Token& directive = advance();	//	consumes SERVER directive

	expectNoArguments();
	expect(TokenType::OPEN_BRACE, directive, "expected \"{\"");

	while (m_pos < m_tokens.size() && !isAtEnd() && peek().type != TokenType::CLOSE_BRACE) {
		// expect KEYWORD
		if (!isValidKeyword(peek()))
			Throw::UnknownOrUnsupportedDirective(peek());

		// select action based on KEYWORD_TYPE
		const Token& directive = peek();
		if (directive.keywordType == KeywordType::LOCATION) {
			LocationBlock location;
			t_parsedDirectives parsedLocationDirectives;
			parseLocationBlock(location, parsedLocationDirectives);

			// check for duplicate location
			for (auto locationRoute : parsedServerDirectives.locationRoutes) {
				if (locationRoute == location.route)
					Throw::DuplicateLocationDirective(directive, location.route);
			}
			parsedServerDirectives.locationRoutes.push_back(location.route);

			server.locationBlocks.emplace_back(location);
		} else {
			parseServerDirectives(server, parsedServerDirectives);
		}
	}
	expect(TokenType::CLOSE_BRACE, directive, "expected \"}\"");
}

void	Parser::parseServerDirectives(ServerBlock& server, t_parsedDirectives& parsedServerDirectives)
{
	const Token& directive = advance();	// grab the current directive token

	// grab all parameters
	std::vector<const Token*>	params;
	while (m_pos < m_tokens.size() && (
	       peek().type == TokenType::PARAM ||
	       peek().type == TokenType::URI ||
	       peek().type == TokenType::NUMBER ||
	       peek().type == TokenType::COLON ||
	       peek().type == TokenType::INVALID)) {
		const Token& current = advance();
		params.emplace_back(&current);
	}
	expect(TokenType::SEMICOLON, directive,  "expected \";\"");

	switch (directive.keywordType) {
		case KeywordType::LISTEN:
			if (parsedServerDirectives.listen)
				Throw::DuplicateListenDirective(directive, server);
			parsedServerDirectives.listen = true;
			parseListenDirective(directive, params, server);
			break;
		case KeywordType::SERVER_NAME:
			parseServerNameDirective(directive, params, server);
			break;
		case KeywordType::ERROR_PAGE:
			parseErrorPageDirective(directive, params, server);
			break;
		case KeywordType::CLIENT_MAX_BODY_SIZE:
			if (parsedServerDirectives.clientMaxBodySize)
				Throw::DuplicateDirective(directive);
			parsedServerDirectives.clientMaxBodySize = true;
			parseClientMaxBodySizeDirective(directive, params, server.clientMaxBodySize);
			break;
		case KeywordType::ROOT:
			if (parsedServerDirectives.root)
				Throw::DuplicateDirective(directive);
			parsedServerDirectives.root = true;
			parseUri(directive, params, server.root);
			break;
		case KeywordType::INDEX:
			parseIndexDirective(directive, params, server.index);
			break;
		default:
			Throw::UnknownOrUnsupportedDirective(directive);
	}
}

void	Parser::parseLocationBlock(LocationBlock& location, t_parsedDirectives& parsedLocationDirectives)
{
	const Token& directive = advance();	//	consumes LOCATION directive

	// grab all parameters
	std::vector<const Token*>	params;
	while (m_pos < m_tokens.size() && (
	       peek().type == TokenType::PARAM ||
	       peek().type == TokenType::URI ||
	       peek().type == TokenType::NUMBER ||
	       peek().type == TokenType::COLON ||
	       peek().type == TokenType::INVALID)) {
		const Token& current = advance();
		params.emplace_back(&current);
	}
	expect(TokenType::OPEN_BRACE, directive, "expected \"{\"");

	//	validate PARAMS
	if (params.size() != 1)
		Throw::InvalidNumberOfArguments(directive);
	if (params[0]->type != TokenType::URI)
		Throw::InvalidValue(directive);

	location.route = params[0]->value;

	while (m_pos < m_tokens.size() && !isAtEnd() && peek().type != TokenType::CLOSE_BRACE) {
		// expect KEYWORD
		if (!isValidKeyword(peek()))
			Throw::UnknownOrUnsupportedDirective(peek());
		parseLocationDirectives(location, parsedLocationDirectives);
	}
	expect(TokenType::CLOSE_BRACE, directive, "expected \"}\"");
}

void	Parser::parseLocationDirectives(LocationBlock& location, t_parsedDirectives& parsedLocationDirectives)
{
	const Token& directive = advance();	// grab the current directive token

	// grab all parameters
	std::vector<const Token*>	params;
	while (m_pos < m_tokens.size() && (
	       peek().type == TokenType::PARAM ||
	       peek().type == TokenType::URI ||
	       peek().type == TokenType::NUMBER ||
	       peek().type == TokenType::COLON ||
	       peek().type == TokenType::INVALID)) {
		const Token& current = advance();
		params.emplace_back(&current);
	}
	expect(TokenType::SEMICOLON, directive,  "expected \";\"");

	switch (directive.keywordType) {
		case KeywordType::CLIENT_MAX_BODY_SIZE:
			if (parsedLocationDirectives.clientMaxBodySize)
				Throw::DuplicateDirective(directive);
			parsedLocationDirectives.clientMaxBodySize = true;
			parseClientMaxBodySizeDirective(directive, params, location.clientMaxBodySize);
			break;
		case KeywordType::ROOT:
			if (parsedLocationDirectives.root)
				Throw::DuplicateDirective(directive);
			parsedLocationDirectives.root = true;
			parseUri(directive, params, location.root);
			break;
		case KeywordType::INDEX:
			parseIndexDirective(directive, params, location.index);
			break;
		case KeywordType::ALLOW_METHODS:
			parseAllowMethodsDirective(directive, params, location);
			break;
		case KeywordType::RETURN:
			parseUri(directive, params, location.returnRoute);
			break;
		case KeywordType::AUTOINDEX:
			if (parsedLocationDirectives.autoIndex)
				Throw::DuplicateDirective(directive);
			parsedLocationDirectives.autoIndex = true;
			parseToggle(directive, params, location.autoIndex);
			break;
		case KeywordType::CGI_EXTENSION:
			parseExtension(directive, params, location.cgiExtension);
			break;
		case KeywordType::ALLOW_UPLOAD:
			if (parsedLocationDirectives.allowUpload)
				Throw::DuplicateDirective(directive);
			parsedLocationDirectives.allowUpload = true;
			parseToggle(directive, params, location.allowUpload);
			break;
		case KeywordType::UPLOAD_STORE:
			if (parsedLocationDirectives.uploadStore)
				Throw::DuplicateDirective(directive);
			parsedLocationDirectives.uploadStore = true;
			parseUri(directive, params, location.uploadDir);
			break;
		default:
			Throw::UnknownOrUnsupportedDirective(directive);
	}
}

void	Parser::parseListenDirective(const Token& directive, std::vector<const Token*>& params, ServerBlock& server)
{
	if (params.size() == 0)
		Throw::InvalidNumberOfArguments(directive);
	if (params[0]->type == TokenType::INVALID)
		Throw::HostNotFound(directive);
	if (params[0]->type == TokenType::URI)
		Throw::InavlidHost(directive, *params[0]);
	if (params[0]->type == TokenType::COLON) {
		if (params.size() > 1 && params[1]->type == TokenType::NUMBER) {
			const Token combinedToken(TokenType::PARAM, std::string(params[0]->value) + params[1]->value, params[0]->line);
			Throw::NoHost(directive, combinedToken);
		}
		Throw::InvalidPort(directive, *params[0]);
	}

	if (params[0]->type == TokenType::PARAM) {
		if (Validator::isIPAddr(params[0]->value)) {
			// we got an IP
			server.listenHostStr = params[0]->value;
			if (server.listenHostStr == "localhost")
				server.listenHostStr = "127.0.0.1";
		} else if (Validator::isDomainName(params[0]->value)) {
			// we got a DomainName
			addrinfo hints;
			hints.ai_family = AF_INET;			// IPv4
			hints.ai_socktype = SOCK_STREAM;	// we want a: stream socket (typically used for TCP)
			addrinfo* res;						// result
			if (getaddrinfo(params[0]->value.c_str(), nullptr, &hints, &res) != 0)
				Throw::HostNotFound(*params[0]);

			// create an IPstring from domain
			sockaddr_in sockaddr;
			sockaddr_in* ipv4 = reinterpret_cast<sockaddr_in*>(res->ai_addr);
			sockaddr = *ipv4;
			freeaddrinfo(res);

			char ipStr[INET_ADDRSTRLEN];
			if (inet_ntop(AF_INET, &(sockaddr.sin_addr), ipStr, sizeof(ipStr)) == nullptr)
				Throw::FailedToConvertDomainToIP(*params[0]);
			server.listenHostStr = ipStr;
		} else 
			Throw::InvalidPort(directive, *params[0]);

		if (params.size() > 1 && params[1]->type == TokenType::COLON)
		{
			//	we got ":" so we have to check for a given PORT
			if (params.size() > 2) {
				if (params[2]->type != TokenType::NUMBER)
					Throw::InvalidPort(directive, *params[2]);
				
				unsigned int port;
				if (!Validator::isValidPort(params[2]->value, port))
					Throw::InvalidPort(directive, *params[2]);
				server.listenPort = port;
			} else {
				const Token combinedToken(TokenType::PARAM, std::string(params[0]->value) + params[1]->value, params[0]->line);
				Throw::InvalidPort(directive, combinedToken);
			}
		}
		else {
			if (params.size() > 1)
				Throw::InvalidParameter(*params[1]);
			server.listenPort = 80;
		}
	} else if (params[0]->type == TokenType::NUMBER) {
		// we got a PORT
		if (params.size() > 1)
			Throw::InvalidParameter(*params[1]);

		unsigned int port;
		if (!Validator::isValidPort(params[0]->value, port))
			Throw::InvalidPort(directive, *params[0]);
		server.listenPort = port;
		server.listenHostStr = "0.0.0.0";
	}
}

void	Parser::parseServerNameDirective(const Token& directive, std::vector<const Token*>& params, ServerBlock& server)
{
	if (params.size() == 0)
		Throw::InvalidNumberOfArguments(directive);

	for (size_t i = 0; i < params.size(); ++i) {
		if (params[i]->type == TokenType::PARAM && Validator::isValidServerName(params[i]->value))
			server.serverNames.emplace_back(params[i]->value);
		else
			Throw::AccpetsOnlyDomainNames(*params[i]);
	}
}

void	Parser::parseErrorPageDirective(const Token& directive, std::vector<const Token*>& params, ServerBlock& server)
{
	if (params.size() != 2)
		Throw::InvalidNumberOfArguments(directive);

	if (params[0]->type != TokenType::NUMBER)
		Throw::InvalidValue(*params[0]);
	if (!(params[1]->type != TokenType::PARAM ||
	      params[1]->type != TokenType::URI ||
	      params[1]->type != TokenType::NUMBER))
		Throw::InvalidValue(*params[1]);

	int error_nbr = 0;
	if (!Validator::isValidErrorPageNbr(params[0]->value, error_nbr))
		Throw::InvalidErrorpageNbr(*params[0]);

	server.errorPagePaths[error_nbr] = params[1]->value;
}

void	Parser::parseAllowMethodsDirective(const Token& directive, std::vector<const Token*>& params, LocationBlock& location)
{
	if (params.size() == 0)
		Throw::InvalidNumberOfArguments(directive);

	location.allowMethods.clear();

	for (size_t i = 0; i < params.size(); ++i) {
		HTTP::Method method;
		if (params[i]->type == TokenType::PARAM && Validator::isValidMethod(params[i]->value, method)) {
			if (std::find(location.allowMethods.begin(), location.allowMethods.end(), method) == location.allowMethods.end())
				location.allowMethods.emplace_back(method);
		}
		else
			Throw::InvalidValue(*params[i]);
	}
}

//	multiscope directive method expects the clientMaxBodySize value, so it can be set in the different scopes
void	Parser::parseClientMaxBodySizeDirective(const Token& directive, std::vector<const Token*>& params, size_t& value)
{
	if (params.size() != 1)
		Throw::InvalidNumberOfArguments(directive);
	if (params[0]->value.find('.') != std::string::npos)
		Throw::InvalidValue(*params[0]);

	size_t bodySizeValue;
	try {
		bodySizeValue = std::stoul(params[0]->value);
		if (params[0]->value.back() == 'k' || params[0]->value.back() == 'K')
			bodySizeValue *= 1024;
		else if (params[0]->value.back() == 'm' || params[0]->value.back() == 'M')
			bodySizeValue *= 1024 * 1024;
		else if (params[0]->value.back() == 'g' || params[0]->value.back() == 'G')
			bodySizeValue *= 1024 * 1024 * 1024;
	} catch(...) {
		Throw::InvalidValue(*params[0]);
	}
	value = bodySizeValue;
}

//	multiscope directive method expects the index string, so it can be set in the different scopes
void	Parser::parseIndexDirective(const Token& directive, std::vector<const Token*>& params, std::string& index)
{
	//	TODO:	we currently go with single param first, since subject doesn't specifiy it as plural, see Notion
	if (params.size() != 1)
		Throw::InvalidNumberOfArguments(directive);
	if (!(params[0]->type == TokenType::PARAM ||
		  params[0]->type == TokenType::URI ||
		  params[0]->type == TokenType::NUMBER))
		Throw::InvalidValue(*params[0]);

	//	TODO:	we currently accept only one parameter
	//			and we accept any PARAM, URI or NUMBER
	index = params[0]->value;
}

//	multiscope directive for URI, expects a single URI as param
void	Parser::parseUri(const Token& directive, std::vector<const Token*>& params, std::string& uri)
{
	if (params.size() != 1)
		Throw::InvalidNumberOfArguments(directive);
	if (params[0]->type != TokenType::URI)
		Throw::InvalidValue(*params[0]);

	//	TODO:	we currently accept any URI
	//			IS this good enough for use ??
	uri = params[0]->value;
}

void	Parser::parseToggle(const Token& directive, std::vector<const Token*>& params, bool& toggle)
{
	if (params.size() != 1)
		Throw::InvalidNumberOfArguments(directive);
	if (params[0]->type != TokenType::PARAM)
		Throw::InvalidValue(*params[0]);

	bool validToggle = toggle;
	if (!Validator::isValidToggle(params[0]->value, validToggle))
		Throw::InvalidValue(*params[0]);

	toggle = validToggle;
}

void	Parser::parseExtension(const Token& directive, std::vector<const Token*>& params, std::string& ext)
{
	if (params.size() != 1)
		Throw::InvalidNumberOfArguments(directive);
	if (params[0]->type != TokenType::PARAM)
		Throw::InvalidValue(*params[0]);

	if (!Validator::isValidExtension(params[0]->value))
		Throw::InvalidExtension(*params[0]);
	
	ext = params[0]->value;
}


//=============================================================================
// Rules and Checks
//=============================================================================


void	Parser::setFallbacks(Config& config)
{
	// set default PORT and LISTEN_HOST_STR
	config.fallback.listenPort    = 80;
	config.fallback.listenHostStr = "0.0.0.0";

	// set default route
	config.fallback.route         = "/";

	//	set default ROOT
	try	{
		// convert argv0 to fs::path
		std::filesystem::path execPath(m_programName);
		execPath.remove_filename();
		// check for absolute
		execPath = std::filesystem::absolute(execPath);
		// resolve symlinks
		execPath = std::filesystem::canonical(execPath);
		config.fallback.root = execPath;
	} catch (...) {
		throw std::runtime_error("failed to resolve executable path to set default value for root directive.");
	}

	// set default for INDEX
	config.fallback.index = "index.html";

	// set default for CLIENT_MAX_BODY_SIZE
	config.fallback.clientMaxBodySize = DEFAULT_CLIENT_MAX_BODY_SIZE;

	// set default for ALLOW_METHODS
	config.fallback.allowMethods.push_back(HTTP::Method::GET);

	// set default for AUTO_INDEX, ALLOW_UPLOAD
	config.fallback.autoIndex   = false;
	config.fallback.allowUpload = false;	
}

void	Parser::checksServerBlocksAndSetsdefaults(Config& config)
{
	for (auto& serverBlock : config.serverBlocks) {
		// listenPort
		if (serverBlock.listenPort == 0)
			serverBlock.listenPort = config.fallback.listenPort;

		// listenHostStr
		if (serverBlock.listenHostStr.empty())
			serverBlock.listenHostStr = config.fallback.listenHostStr;

		// clientMaxBodySize
		// is set by default to DEFAULT_CLIENT_MAX_BODY_SIZE

		// root
		if (serverBlock.root.empty())
			serverBlock.root = config.fallback.root;

		// index
		if (serverBlock.index.empty())
			serverBlock.index = config.fallback.index;

		// set a default location if we dont have one
		auto it = std::find_if(serverBlock.locationBlocks.begin(),
		                       serverBlock.locationBlocks.end(),
							   [](const LocationBlock& loc){ return loc.route == "/"; });
		if (it == serverBlock.locationBlocks.end()) {
			LocationBlock locationBlock;
			locationBlock.route             = config.fallback.route;
			locationBlock.allowMethods      = config.fallback.allowMethods;
			locationBlock.autoIndex         = config.fallback.autoIndex;
			locationBlock.allowUpload       = config.fallback.allowUpload;
			locationBlock.clientMaxBodySize = serverBlock.clientMaxBodySize;
			locationBlock.index             = serverBlock.index;
			locationBlock.root              = serverBlock.root;
			// and add the new default location to this serverBlock
			serverBlock.locationBlocks.push_back(locationBlock);
		}

		// and copy down all serverBlock values into all locations for easy safe access
		for (auto& locationBlock : serverBlock.locationBlocks) {
			if (locationBlock.allowMethods.size() == 0)
				locationBlock.allowMethods = config.fallback.allowMethods;
			if (locationBlock.clientMaxBodySize == std::numeric_limits<size_t>::max())
				locationBlock.clientMaxBodySize = serverBlock.clientMaxBodySize;
			if (locationBlock.index.empty())
				locationBlock.index = serverBlock.index;
			if (locationBlock.root.empty())
				locationBlock.root = serverBlock.root;
		}

		// and sort the locations, longes route first
		std::sort(serverBlock.locationBlocks.begin(), serverBlock.locationBlocks.end(),
		          [](LocationBlock& a, LocationBlock& b){
				      return a.route.length() > b.route.length();
				  });

	}
}
