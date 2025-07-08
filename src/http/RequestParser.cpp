#include "RequestParser.hpp"
#include <sstream>
#include <algorithm>
#include <unordered_set>
#include "HTTP.hpp"
#include "Validator.hpp"
#include "Log.hpp"
#include "Utils.hpp"
#include "webserv.hpp"

void	RequestParser::parseNext(Request& request)
{
	LOGT(Log::INFO, "===> RequestParser::parseNext");
	if (request.parsingState == Request::ParsingState::REQUEST_LINE) {
		LOGT(Log::INFO, "---> Reqeust: parse requestLine");
		parseRequestLine(request);
	}
	if (request.parsingState == Request::ParsingState::HEADERS) {
		LOGT(Log::INFO, "---> Request: parser headers");
		parseHeader(request);
	}
	if (request.parsingState == Request::ParsingState::BODY) {
		LOGT(Log::INFO, "---> Request: parser body");
		parseBody(request);
	}
	if (request.parsingState == Request::ParsingState::COMPLETE || request.parsingState == Request::ParsingState::INVALID) {
		LOGT(Log::INFO, "---> Request: " << (request.parsingState == Request::ParsingState::COMPLETE ? "COMPLETE" : "INVALID"));
		request.doneReceiving = true;
	}
}

void	RequestParser::parseRequestLine(Request& request)
{
	size_t	requestLineEnd = request.rawRequest.find("\r\n");
	if (requestLineEnd == std::string::npos) {
		// waiting until we have recieved the whole requestLine
		return;
	}

	// read the elements of the requestLine
	std::istringstream lineStream(request.rawRequest.substr(0, requestLineEnd).c_str());
	std::string methodStr;
	if (!std::getline(lineStream, methodStr, ' ') ||
	    !std::getline(lineStream, request.requestTarget, ' ') ||
		!std::getline(lineStream, request.protokoll, ' ')) {
		request.errorStatusCode = WSSC_INTERNAL_SERVER_ERROR;
		request.parsingState = Request::ParsingState::INVALID;
		return;
	}

	// Don't allow extra garbage after the protokoll in the request line
	std::string someExtraGarbage;
	if (lineStream >> someExtraGarbage) {
		request.errorStatusCode = WSSC_BAD_REQUEST;
		request.parsingState = Request::ParsingState::INVALID;
		return;
	}

	LOG("---> raw requestLine:       " << LIGHTRED << HTTP::methodToString(request.method)
		<< " " << LIGHTGREEN << request.requestTarget << " " << LIGHTBLUE
		<< request.protokoll);

	if (!validateHttpMethod(methodStr, request)) {
		request.errorStatusCode = WSSC_BAD_REQUEST;
		request.parsingState = Request::ParsingState::INVALID;
		return;
	}

	if (!validateRequestTarget(request)) {
		request.errorStatusCode = WSSC_BAD_REQUEST;
		request.parsingState = Request::ParsingState::INVALID;
		return;
	}

	if (!validateProtokoll(request)) {
		request.errorStatusCode = WSSC_HTTP_VERSION_NOT_SUPPORTED;
		request.parsingState = Request::ParsingState::INVALID;
		return;
	}

	LOG("---> validated requestLine: " << LIGHTRED << HTTP::methodToString(request.method)
		<< " " << LIGHTGREEN << request.URI << " " << LIGHTBLUE << request.protokoll);

	request.rawRequest.erase(0, requestLineEnd + 2);
	request.parsingState = Request::ParsingState::HEADERS;
}

void	RequestParser::parseHeader(Request& request)
{
	size_t headerEnd  = request.rawRequest.find("\r\n\r\n");

	if (headerEnd == std::string::npos) {
		// we need to wait until we have recieved the whole header
		return;
	}

	size_t	start = 0;
	while (start < headerEnd) {
		size_t	pos = request.rawRequest.find_first_of('\n', start);

		if (pos == std::string::npos) {
			request.errorStatusCode = WSSC_BAD_REQUEST;
			request.parsingState = Request::ParsingState::INVALID;
			return;
		}

		std::string line = request.rawRequest.substr(start, pos - start);

		// clean trailing '\r' character
		if (!line.empty() && line.back() == '\r') 
			line.pop_back();

		if (line.empty()) {
			request.errorStatusCode = WSSC_BAD_REQUEST;
			request.parsingState = Request::ParsingState::INVALID;
			return;
		}

		std::pair<std::string, std::string> headerField;
		if (!splitHeaderField(line, headerField)) {
			LOGT(Log::WARNING, "failed to split Header-Field: " << line);
			request.errorStatusCode = WSSC_BAD_REQUEST;
			request.parsingState = Request::ParsingState::INVALID;
			return;
		}

		std::transform(headerField.first.begin(), headerField.first.end(),
		               headerField.first.begin(),
					   [](unsigned char c){ return std::tolower(c); });

		request.headers[headerField.first] = headerField.second;

		start = pos + 1;
	}
	request.rawRequest.erase(0, headerEnd + 4);

	// first we resolve the serverBlock according to the value of HOST
	if (!resolveServerBlock(request))
		return;

	if (!validateRequiredHeaderFields(request))
		return;

	if (!resolveRequestContext(request))
		return;

	request.parsingState = Request::ParsingState::BODY;
}

void	RequestParser::parseBody(Request& request)
{
	// TODO: Implement
	request.parsingState = Request::ParsingState::COMPLETE;
}

bool	RequestParser::validateHttpMethod(std::string& methodStr, Request& request)
{
	if (Validator::isValidMethod(methodStr, request.method))
		return true;

	std::string ustr = methodStr;
	std::transform(ustr.begin(), ustr.end(), ustr.begin(),
	               [](unsigned char c){ return (std::toupper(c)); });

	if (ustr == "GET\0" || ustr == "POST\0" || ustr == "DELETE\0") {
		request.method = HTTP::strToMethod(methodStr);
		request.errorStatusCode = WSSC_BAD_REQUEST;
		request.parsingState = Request::ParsingState::INVALID;
		return false;
	}

	request.errorStatusCode = WSSC_METHOD_NOT_ALLOWED;
	request.parsingState = Request::ParsingState::INVALID;
	return false;
}

bool	RequestParser::validateRequestTarget(Request& request)
{
	// reject asterisk-form and absolute-form and return early if we don't have a '/'
	if (request.requestTarget == "*" ||
		request.requestTarget.find("://") != std::string::npos ||
		request.requestTarget.find('/') != 0) {
		request.errorStatusCode = WSSC_BAD_REQUEST;
		request.parsingState = Request::ParsingState::INVALID;
		return false;
	}

	if (!validRawCharacters(request.requestTarget) ||
		!percentDecoding(request.requestTarget, request.URI) ||
		!validDecodedCharacters(request.URI) ||
	    !isRelativeForm_EnsureLeadingSlash(request.URI) ||
	    !Utils::removeDotSegments(request.URI) ||
		!Utils::collapseDuplicateSlashes(request.URI)) {
		request.errorStatusCode = WSSC_BAD_REQUEST;
		request.parsingState = Request::ParsingState::INVALID;
		return false;
	}

	if (request.URI.empty() ||
		request.URI.front() != '/' ||
		request.URI.find('\\') != std::string::npos) {
		request.errorStatusCode = WSSC_BAD_REQUEST;
		request.parsingState = Request::ParsingState::INVALID;
		return false;
	}

	if (request.URI.size() > MAX_URI_LENGTH) {
		request.errorStatusCode = WSSC_URI_TOO_LONG;
		request.parsingState = Request::ParsingState::INVALID;
		return false;
	}

	return true;
}

bool	RequestParser::validateProtokoll(Request& request)
{
	if (request.protokoll == "HTTP/1.1")
		return true;

	if (request.protokoll == "HTTP/1.0") {
		request.protokoll = "HTTP/1.1";
		return true;
	}
	return false;
}

bool	RequestParser::validRawCharacters(const std::string& requestTarget)
{
	for (auto it = requestTarget.cbegin(); it < requestTarget.cend(); ++it) {
		if (*it <= 0x1F || *it == 0x7F || *it == ' ') {
			LOG_DEBUG("failed validRawCharacters(): " + Utils::charToHex(*it));
			return false;
		}
	}
	return true;
}

bool	RequestParser::validDecodedCharacters(const std::string& uri)
{
	for (auto it = uri.cbegin(); it < uri.cend(); ++it) {
		if (*it <= 0x1F || *it == 0x7F || *it == '\\') {
			LOG_DEBUG("failed validDecodedCharacters(): " + Utils::charToHex(*it));
			return false;
		}

		// This is very strict because of filesystem restrictions, but RFC 3986 2.2 allows
		if (*it == '\"' || *it == '<' || *it == '>' ||
			*it == '^' || *it == '`' || *it == '{' || *it == '}' ||
			*it == '|') {
			LOG_DEBUG("failed validDecodedCharacters(): " + Utils::charToHex(*it));
			return false;
		}
	}
	return true;
}

const std::string	RequestParser::truncateQueryAndFragments(const std::string& requestTarget)
{
	size_t pos = requestTarget.find_first_of("?#");
	if (pos != std::string::npos)
		return requestTarget.substr(0, pos);
	return requestTarget;
}

bool	RequestParser::percentDecoding(const std::string& requestTarget, std::string& destURI)
{
	const std::string truncTarget = truncateQueryAndFragments(requestTarget);
	destURI.reserve(truncTarget.size());

	for (size_t idx = 0; idx < truncTarget.size(); ++idx) {
		if (truncTarget[idx] == '%') {
			if (idx + 2 < truncTarget.size() &&
			    std::isxdigit(static_cast<unsigned char>(truncTarget[idx + 1])) &&
			    std::isxdigit(static_cast<unsigned char>(truncTarget[idx + 2]))) {
					// Valid percent-encoding: decode and append character
					char hiHex = truncTarget[++idx];
					char loHex = truncTarget[++idx];
					destURI += static_cast<char>((hexToInt(hiHex) << 4) | hexToInt(loHex));
			} else {
				// invalid Hex character, or too short for valid hex form: xFF
				return false;
			}
		} else {
			destURI += truncTarget[idx];
		}
	}
	return true;
}

int	RequestParser::hexToInt(const char c)
{
	return std::isdigit(c) ? c - '0' : std::tolower(c) - 'a' + 10;
}

bool	RequestParser::isRelativeForm_EnsureLeadingSlash(std::string& uri)
{
	if (Utils::startsWith(uri, "http://") || Utils::startsWith(uri, "https://"))
		return false;

	// make URI absolute per RFC 7230: Section 5.3.1
	if (!uri.empty() && uri[0] != '/')
		uri.insert(0, "/");

	return true;
}

bool	RequestParser::splitHeaderField(std::string& line, std::pair<std::string, std::string>& headerField)
{
	// no whitespace allowed before the field-name
	if (line.size() > 0 && std::isspace(line[0]))
		return false;

	// has valid ':'
	size_t pos = line.find_first_of(":");
	if (pos == std::string::npos)
		return false;
	
	// check if field-name has valid chars of tchar: RFC 7230: 3.2.6.
	for (size_t i = 0; i < pos; ++i) {
		if (!isValidFieldNameChar(line[i]))
			return false;		              
	}
	
	headerField.first = line.substr(0, pos);
	headerField.second = line.substr(pos + 1);

	// trim whitespaces from front and back ONLY for field-value
	Utils::trimWhitespaces(headerField.second);
	// validate field-value after trimming
	for (auto it = headerField.second.cbegin(); it < headerField.second.cend(); ++it) {
		if (!isValidFieldValueChar(*it))
			return false;		              
	}

	return true;
}

bool	RequestParser::isValidFieldNameChar(const char c)
{
	static const std::unordered_set<char> tChars = {
		'!', '#' , '$', '%', '&', '\'', '*', '+',
		'-', '.' , '^', '_', '`', '|',  '~'
	};
	return std::isalnum(static_cast<unsigned char>(c)) || tChars.count(c);
}

bool	RequestParser::isValidFieldValueChar(const char c)
{
	const unsigned char uc = static_cast<unsigned char>(c);
	return (uc >= ' ' && uc <= '~') || uc == '\t';
}

bool	RequestParser::validateRequiredHeaderFields(Request& request)
{
	// check content-length if available on valid field-value
	auto it = request.headers.find("content-length");
	if (it != request.headers.end()) {
		const std::string contentLengthStr = it->second;
		for (auto& c : contentLengthStr) {
			if (!std::isdigit(c)) {
				request.errorStatusCode = WSSC_BAD_REQUEST;
				request.parsingState = Request::ParsingState::INVALID;
				return false;
			}
		}

		try {
			size_t resolvedContentLength = std::stoull(contentLengthStr);

			//	TODO:	use resolved clientMaxBodySize
			if (resolvedContentLength > request.resolvedServerBlock->clientMaxBodySize) {
				request.errorStatusCode = WSSC_CONTENT_TOO_LARGE;
				request.parsingState = Request::ParsingState::INVALID;
				return false;
			}
			request.contentLength = resolvedContentLength;
		} catch(...) {
			request.errorStatusCode = WSSC_BAD_REQUEST;
			request.parsingState = Request::ParsingState::INVALID;
			return false;
		}
	}

	// check transfer-encoding
	it = request.headers.find("transfer-encoding");
	if (it != request.headers.end()) {
		//	INFO:	here we only allow "transfer-encoding:chunked"
		//			we could accept "transfer-encoding:gzip, chunked, deflate"
		if (it->second != "chunked") {
			request.errorStatusCode = WSSC_NOT_IMPLEMENTED;
			request.parsingState = Request::ParsingState::INVALID;
			return false;
		}
	}

	return true;
}

bool	RequestParser::validateHost(Request& request, std::string& dest)
{
	auto hostIT = request.headers.find("host");
	if (hostIT == request.headers.end())
		return false;

	// down case host and strip the port if found
	std::string hostSTR = hostIT->second;
	size_t colonPos = hostSTR.find(":");
	if (colonPos != std::string::npos)
		hostSTR = hostSTR.substr(0, colonPos);
	std::transform(hostSTR.begin(), hostSTR.end(), hostSTR.begin(),
	               [](unsigned char c){ return std::tolower(c); });

	// Browsers do not send Host lists, but we check that we have
	// a valid Host which should be a valid DomainName
	if (!(Validator::isDomainName(hostSTR) ||
	      Validator::isIPAddr(hostSTR) ||
		  hostSTR == "localhost"))
		return false;

	dest = hostSTR;
	return true;
}


//=============================================================================
// Resolve Request Context
//=============================================================================

bool	RequestParser::resolveServerBlock(Request& request)
{
	// we dont have any serverBlocks this should never happen
	// This is a safety fallback
	if (request.serverBlocks.empty()) {
		request.errorStatusCode = WSSC_INTERNAL_SERVER_ERROR;
		request.parsingState = Request::ParsingState::INVALID;
		return false;
	}

	// checking for available and valid HOST
	std::string hostStr;
	if  (!validateHost(request, hostStr)) {
		request.errorStatusCode = WSSC_BAD_REQUEST;
		request.parsingState = Request::ParsingState::INVALID;
		return false;
	}

	for (auto& serverBlock : request.serverBlocks) {
		for (const auto& name : serverBlock.serverNames) {
			if (name == hostStr) {
				// we found our serverBlock
				request.resolvedServerBlock = &serverBlock;
				return true;
			}
		}
	}

	// we take the first serverBlock as default	if no server_name matches the host
	LOG("no host '" << hostStr << "' matches any server_name: we take the first serverBlock as default");
	request.resolvedServerBlock = &request.serverBlocks.front();
	return true;
}

bool	RequestParser::resolveRequestContext(Request& request)
{
	// resolve LocationBlock
	if (!resolveLocationBlock(request)) {
		// This would be a critical ERROR, we should alwys find a locationBlock.
		// But if sth bad happen, we return "internal server error"
		LOGT(Log::WARNING, "something went terrible wrong while matching locations: failed to find default \"/\" location in RequestParser::resolveLocationBlock()");
		request.errorStatusCode = WSSC_INTERNAL_SERVER_ERROR;
		request.parsingState = Request::ParsingState::INVALID;
		return false;
	}

	//	all the config attributes are now resolved, and we can use:
	//	ServerBlock
			// listenPort, listenHostStr, serverNames, errorPagePaths
	//	LocationBlock
			// route, root, index, clientmaxBodySize, allowMethods, returnRoute,
			// autoIndex, cgiExtension, allowUpload, uploadDir

	//	resolve path
	if (!resolvePath(request)) {
		return false;
	}

	return true;
}

bool	RequestParser::resolveLocationBlock(Request& request)
{
	// TODO: use the resolved serverBlock
	const LocationBlock* bestMatch = nullptr;
	for (const LocationBlock& loc : request.resolvedServerBlock->locationBlocks) {
		if (Utils::startsWith(request.URI, loc.route)) {
			if (bestMatch == nullptr || bestMatch->route.length() < loc.route.length())
				bestMatch = &loc;
		}
	}
	if (bestMatch == nullptr) {
		auto it = std::find_if(request.resolvedServerBlock->locationBlocks.begin(), request.resolvedServerBlock->locationBlocks.end(),
		                     [](const LocationBlock& loc){ return loc.route == "/"; });
		if (it == request.resolvedServerBlock->locationBlocks.end())
			return false;
		bestMatch = &(*it);
	}
	request.resolvedLocationBlock = const_cast<LocationBlock*>(bestMatch);
	return true;
}

bool	RequestParser::resolvePath(Request& request)
{
	// ensure root path has not a '/'
	std::string root = request.resolvedLocationBlock->root;
	if (root.back() == '/')
		root.pop_back();

	// ensure we have a leading '/' doube check should we eliminate this check?
	if (request.URI.front() != '/')
		return false;

	//	TODO:	if we dont sanatize the resolvedPath for /../ or /./ or /////
	//			we can set it directly to request.resolvedPath
	std::string resolvedPath = root + request.URI;

	//	TODO:	do we want to sanatize and clean the root and resolvedPath??

	LOGT(Log::INFO, LIGHTMAGENTA << "resolvedPath: " << LIGHTGREEN << BOLD << resolvedPath);
	request.resolvedPath = resolvedPath;
	return true;
}
