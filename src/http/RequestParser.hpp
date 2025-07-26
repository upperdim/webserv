#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include <string>
#include "colors.hpp"
#include "Request.hpp"

#define REQUEST_MAX_URI_LENGTH	2048

class RequestParser
{
public:
	static void					parseNext(Request& request);

private:
	static void					parseRequestLine(Request& request);
	static void					parseHeader(Request& request);
	static void					parseBody(Request& request);
	static void					parseMultiformBody(Request& request);
	static bool					streamMultipartPartToFile(std::ifstream& ifs, std::ofstream& ofs, const std::string& boundary);

	// Request line helpers
	static bool					validateHttpMethod(std::string& methodStr, Request& request);
	static bool					validateRequestTarget(Request& request);
	static bool					validateProtokoll(Request& request);

	// Request line -> Request target helpers
	static bool					validRawRequestTargetChars(const std::string& requestTarget);
	static bool					validDecodedRequestTargetChars(const std::string& uri);
	static void					parseQueryString(const std::string& sourceURI, std::string& destQueryString);
	static void					truncateQueryAndFragments(std::string& URI);
	static bool					percentDecoding(const std::string& requestTarget, std::string& destURI);
	static int					hexToInt(const char c);
	static bool					isRelativeForm_EnsureLeadingSlash(std::string& uri);

	// Header helpers
	static bool					validateOptionalHeaderFields(Request& request);
	static bool					validateHost(Request& request, std::string& dest);

	// Header -> Resolve request context helpers
	static bool					resolveServerBlock(Request& request);
	static bool					resolveRequestContext(Request& request);
	static bool					resolveLocationBlock(Request& request);
	static bool					resolvePath(Request& request);

	// Body helpers
	static void					storeContentLengthBody(Request& request);
	static void					storeChunkedTransferBody(Request& request);
};

#endif
