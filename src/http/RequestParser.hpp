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
	static bool					isFileUploadRequest(const Request& request);
	static void					storeContentLengthBody(Request& request);
	static void					storeChunkedTransferBody(Request& request);

	static bool					validateHttpMethod(std::string& methodStr, Request& request);
	static bool					validateRequestTarget(Request& request);
	static bool					validateProtokoll(Request& request);

	static bool					validRawRequestTargetChars(const std::string& requestTarget);
	static bool					validDecodedRequestTargetChars(const std::string& uri);
	static const std::string	truncateQueryAndFragments(const std::string& requestTarget);
	static bool					percentDecoding(const std::string& requestTarget, std::string& destURI);
	static int					hexToInt(const char c);
	static bool					isRelativeForm_EnsureLeadingSlash(std::string& uri);

	static bool					readHeaders(Request& request, const size_t headerEnd, std::unordered_map<std::string, std::string>& headers);

	static bool					validateOptionalHeaderFields(Request& request);
	static bool					validateHost(Request& request, std::string& dest);

	static bool					resolveServerBlock(Request& request);
	static bool					resolveRequestContext(Request& request);
	static bool					resolveLocationBlock(Request& request);
	static bool					resolvePath(Request& request);

};

#endif
