#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <unordered_map>
#include <optional>
#include "HTTP.hpp"
#include "Config.hpp"

class Request
{
public:
	Request(const ServerBlock& _serverBlock);

	enum class ParsingState {
		REQUEST_LINE,
		HEADERS,
		BODY,
		COMPLETE,
		INVALID
	};

	void					setError(int _statusCode);
	bool					error(void);

	const LocationBlock&	locationBlock();
	bool					isAllowedMethod(void);

	std::string										rawRequest;
	ParsingState									parsingState;
	bool											doneReceiving;
	
	// raw request attributes
	HTTP::Method									method;
	std::optional<int>								errorStatusCode;
	std::string										requestTarget;
	std::string										protokoll;
	std::unordered_map<std::string, std::string>	headers;

	const ServerBlock&								serverBlock;

	// matched and resolved attributes
	std::string										URI;		//	decoded and sanatized requesttarget
	// resolved from header
	std::optional<size_t>							contentLength;

private:
	bool											m_error;
	LocationBlock*									m_locationBlock;
};

#endif
