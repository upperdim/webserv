#include "RequestParser.hpp"
#include <algorithm>
#include <filesystem>
#include <sstream>
#include "HTTP.hpp"
#include "RequestHandler.hpp"
#include "Validator.hpp"
#include "Log.hpp"
#include "Utils.hpp"

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

	LOG("---> raw requestLine:       " << LIGHTRED << "UNDEFINED_METHOD"
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

		std::pair<std::string, std::string> headerField;
		if (!Utils::splitHeaderLine(line, headerField)) {
			request.errorStatusCode = WSSC_BAD_REQUEST;
			request.parsingState = Request::ParsingState::INVALID;
			return;
		}

		request.headers[headerField.first] = headerField.second;

		start = pos + 1;
	}
	// delete the read header Bytes from rawRequest
	request.rawRequest.erase(0, headerEnd + 4);

	// first we resolve the serverBlock according to the value of HOST
	if (!resolveServerBlock(request))
		return;

	if (!resolveLocationBlock(request)) {
		// This would be a critical ERROR, we should always find a locationBlock. But if sth bad happen, we return "internal server error"
		LOGT(Log::WARNING, "something went terrible wrong while matching locations: failed to find default \"/\" location in RequestParser::resolveLocationBlock()");
		request.errorStatusCode = WSSC_INTERNAL_SERVER_ERROR;
		request.parsingState = Request::ParsingState::INVALID;
		return;
	}
	//	all the config attributes are now resolved, and we can use:
	//	ServerBlock
			// listenPort, listenHostStr, serverNames
	//	LocationBlock
			// route, root, index, clientmaxBodySize, errorPagePaths, allowMethods,
			// returnRoute, autoIndex, cgiExtension, allowUpload, uploadDir

	if (!validateOptionalHeaderFields(request))
		return;

	if (!resolvePath(request)) {
		// This should not happen
		request.errorStatusCode = WSSC_BAD_REQUEST;
		request.parsingState = Request::ParsingState::INVALID;
		return;
	}

	if (request.hasBody()) {
		if (!request.createTempBodyFile()) {
			request.errorStatusCode = WSSC_INTERNAL_SERVER_ERROR;
			request.parsingState = Request::ParsingState::INVALID;
			return;
		}
	}

	request.parsingState = Request::ParsingState::BODY;
}

void	RequestParser::parseBody(Request& request)
{
	if (!request.hasBody()) {
		request.parsingState = Request::ParsingState::COMPLETE;
		return;
	}

	// We support any kind of (content type) request body for requests which target CGI script URIs
	if (request.isCGIRequest()) {
		// We still need to unchunk the body before passing it to the script
		if (request.isChunkedBodyTransfer) {
			storeChunkedTransferBody(request);
		} else {
			storeContentLengthBody(request);
		}

		return;
	}

	// We support file upload request bodies on everywhere
	if (request.isFileUploadRequest()) {
		if (request.isChunkedBodyTransfer) {
			storeChunkedTransferBody(request);
		} else {
			storeContentLengthBody(request);
		}

		// Above function just completed storing the body
		if (request.parsingState == Request::ParsingState::COMPLETE) {
			if (!request.contentType.has_value() || !request.contentType.value().boundary.has_value()) {
				request.errorStatusCode = WSSC_BAD_REQUEST;
				request.parsingState = Request::ParsingState::INVALID;
				return;
			}
			// we got the body, now we extract the files
			parseMultiformBody(request);
			return;
		}

		if (request.parsingState == Request::ParsingState::BODY)
			return;
	}

	// We don't support any other type of request body
	request.errorStatusCode = WSSC_BAD_REQUEST;
	request.parsingState = Request::ParsingState::INVALID;
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

	if (!validRawRequestTargetChars(request.requestTarget) ||
		!percentDecoding(request.requestTarget, request.URI)) {
		request.errorStatusCode = WSSC_BAD_REQUEST;
		request.parsingState = Request::ParsingState::INVALID;
		return false;
	}

	parseQueryString(request.URI, request.queryString);
	truncateQueryAndFragments(request.URI);

	if (!validDecodedRequestTargetChars(request.URI) ||
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

	if (request.URI.size() > REQUEST_MAX_URI_LENGTH) {
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

bool	RequestParser::validRawRequestTargetChars(const std::string& requestTarget)
{
	for (auto it = requestTarget.cbegin(); it < requestTarget.cend(); ++it) {
		if (*it <= 0x1F || *it == 0x7F || *it == ' ') {
			LOG_DEBUG("failed validRawCharacters(): " + Utils::charToHex(*it));
			return false;
		}
	}
	return true;
}

bool	RequestParser::validDecodedRequestTargetChars(const std::string& uri)
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

void	RequestParser::parseQueryString(const std::string& sourceURI, std::string& destQueryString)
{
	size_t queryStartIdx = sourceURI.find("?");
	if (queryStartIdx == std::string::npos && sourceURI.size() >= queryStartIdx + 1) {
		destQueryString = "";
	} else {
		destQueryString = sourceURI.substr(queryStartIdx + 1);
	}
}

void	RequestParser::truncateQueryAndFragments(std::string& URI)
{
	size_t pos = URI.find_first_of("?#");
	if (pos != std::string::npos)
		URI = URI.substr(0, pos);
}

bool	RequestParser::percentDecoding(const std::string& requestTarget, std::string& destURI)
{
	destURI.reserve(requestTarget.size());

	for (size_t idx = 0; idx < requestTarget.size(); ++idx) {
		if (requestTarget[idx] == '%') {
			if (idx + 2 < requestTarget.size() &&
			    std::isxdigit(static_cast<unsigned char>(requestTarget[idx + 1])) &&
			    std::isxdigit(static_cast<unsigned char>(requestTarget[idx + 2]))) {
					// Valid percent-encoding: decode and append character
					char hiHex = requestTarget[++idx];
					char loHex = requestTarget[++idx];
					destURI += static_cast<char>((hexToInt(hiHex) << 4) | hexToInt(loHex));
			} else {
				// invalid Hex character, or too short for valid hex form: xFF
				return false;
			}
		} else {
			destURI += requestTarget[idx];
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
	if (Utils::strStartsWith(uri, "http://") || Utils::strStartsWith(uri, "https://"))
		return false;

	// make URI absolute per RFC 7230: Section 5.3.1
	if (!uri.empty() && uri[0] != '/')
		uri.insert(0, "/");

	return true;
}

bool	RequestParser::validateOptionalHeaderFields(Request& request)
{
	// check content-length if available on valid field-value
	auto itCL = request.headers.find("content-length");
	if (itCL != request.headers.end()) {
		const std::string contentLengthStr = itCL->second;
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
			if (resolvedContentLength > request.resolvedLocationBlock->clientMaxBodySize) {
				LOGT(Log::WARNING, "Content-Length too large: " << resolvedContentLength << " | client_max_body_size: " << request.resolvedServerBlock->clientMaxBodySize);
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

	// check content-type
	auto it = request.headers.find("content-type");
	if (it != request.headers.end()) {
		bool error = false;
		request.contentType = HTTP::getContentTypeInfo(it->second, error);
		if (error) {
			request.errorStatusCode = WSSC_BAD_REQUEST;
			request.parsingState = Request::ParsingState::INVALID;
			return false;
		}

		if (request.contentType.has_value()) {
			const HTTP::ContentTypeInfo_t& contentType = request.contentType.value();
			std::string boundary = contentType.boundary.value_or("");

			LOGTL(Log::INFO, "content-type",
				LIGHTYELLOW << "\nraw:" << contentType.raw
				<< "\nType:" << static_cast<int>(contentType.type)
				<< "\nboundary:" << boundary);

			// validate boundary for multipart/form-data
			if (contentType.type == HTTP::ContentType::MULTIPART_FORM_DATA &&
				!Validator::isValidContentTypeBoundary(boundary)) {
				request.errorStatusCode = WSSC_BAD_REQUEST;
				request.parsingState = Request::ParsingState::INVALID;
				return false;
			}
			// set the quote striped boundery back the original request attribute
			request.contentType.value().boundary = boundary;
		}
	}

	// check transfer-encoding
	auto itTE = request.headers.find("transfer-encoding");
	if (itTE != request.headers.end()) {
		//	INFO:	here we only allow "transfer-encoding:chunked"
		//			we could accept "transfer-encoding:gzip, chunked, deflate"
		if (itTE->second != "chunked") {
			request.errorStatusCode = WSSC_NOT_IMPLEMENTED;
			request.parsingState = Request::ParsingState::INVALID;
			return false;
		}
		request.isChunkedBodyTransfer = true;
	}

	// check that we only allow one: content_length || transfer-encoding
	if (itCL != request.headers.end() && itTE != request.headers.end()) {
		request.errorStatusCode = WSSC_BAD_REQUEST;
		request.parsingState = Request::ParsingState::INVALID;
		return false;
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

bool	RequestParser::resolveLocationBlock(Request& request)
{
	const LocationBlock* bestMatch = nullptr;
	for (const LocationBlock& loc : request.resolvedServerBlock->locationBlocks) {
		if (Utils::strStartsWith(request.URI, loc.route)) {
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

void	RequestParser::storeContentLengthBody(Request& request)
{
	if (request.headers.find("content-length") != request.headers.end()) {
		LOGT(Log::DEBUG, "Storing Content-Length: " << request.headers.at("content-length") << " request body");
	} else {
		LOGT(Log::DEBUG, "ERROR: Could not access request header \"content-length\" in storeContentLengthBody()");
	}

	request.bodyFile.write(request.rawRequest.data(), request.rawRequest.size());
	request.bodyBytesStored += request.rawRequest.size();

	LOGT(Log::DEBUG, "written " << request.rawRequest.size() << " bytes (total written: " << request.bodyBytesStored << " bytes)");

	request.rawRequest.erase(0, request.rawRequest.size());

	if (request.bodyBytesStored >= request.contentLength) {
		LOGT(Log::DEBUG, "Storing body: COMPLETE (total written: " << request.bodyBytesStored << " bytes)");
		request.bodyFile.close();
		request.parsingState = Request::ParsingState::COMPLETE;
	}
}

// Chunked request:
// <Request line>\r\n
// <Headers>\r\n
// \r\n
// <chunk size in hex>\r\n<chunk data>\r\n
// ...
// Final chunk: 0\r\n\r\n
void	RequestParser::storeChunkedTransferBody(Request& request)
{
	LOGT(Log::DEBUG, "Storing Transfer-Encoding: chunked request body");

	while (!request.rawRequest.empty()) {
		if (request.awaitingChunkSize) {
			// Look for chunk size line ending in \r\n
			size_t pos = request.rawRequest.find("\r\n");
			if (pos == std::string::npos) {
				// Wait for more data
				return;
			}

			std::string chunkSizeStr = request.rawRequest.substr(0, pos);
			std::stringstream ss;
			ss << std::hex << chunkSizeStr;
			ss >> request.currentChunkSize;

			request.rawRequest.erase(0, pos + 2);  // Remove chunk size line + \r\n

			if (request.currentChunkSize == 0) {
				// Last chunk
				size_t trailerEnd = request.rawRequest.find("\r\n");
				if (trailerEnd != std::string::npos) {
					request.parsingState = Request::ParsingState::COMPLETE;
					request.bodyFile.close();
				}
				return;
			}

			request.awaitingChunkSize = false;
			request.currentChunkBytesReceived = 0;
		} else {
			// Processing chunk data

			// Calculate how many bytes we can write now
			size_t remainingChunkBytes = request.currentChunkSize - request.currentChunkBytesReceived;
			size_t bytesAvailable = request.rawRequest.size();

			// If enough bytes available for full chunk
			if (bytesAvailable >= remainingChunkBytes + 2) {
				// Write chunk data
				request.bodyFile.write(request.rawRequest.data(), remainingChunkBytes);
				request.currentChunkBytesReceived += remainingChunkBytes;

				// Skip chunk data + trailing \r\n
				request.rawRequest.erase(0, remainingChunkBytes + 2);

				// Ready to read next chunk size
				request.awaitingChunkSize = true;
			} else {
				// Not enough data yet
				size_t toWrite = std::min(remainingChunkBytes, bytesAvailable);
				request.bodyFile.write(request.rawRequest.data(), toWrite);
				request.currentChunkBytesReceived += toWrite;
				request.rawRequest.erase(0, toWrite);
				return;  // Wait for more data
			}
		}
	}
}

void	RequestParser::parseMultiformBody(Request& request)
{
	std::ifstream ifs(request.bodyTempFilename, std::ios::in | std::ios::binary);
	if (!ifs) {
		LOGT(Log::ERROR, "Failed to open request bodyFile: " << request.bodyTempFilename);
		request.errorStatusCode = WSSC_INTERNAL_SERVER_ERROR;
		request.parsingState = Request::ParsingState::INVALID;
		return;
	}

	const std::string& boundary = request.contentType->boundary.value();
	const std::string boundaryMarker = "--" + boundary;
	const std::string finalBoundaryMarker = boundaryMarker + "--";
	std::string line;
	std::unordered_map<std::string, std::string> multipartHeaders;

	while (std::getline(ifs, line)) {
		if (!line.empty() && line.back() == '\r')
			line.pop_back();

		// skip lines until we reach a boundary
		if (line != boundaryMarker && line != finalBoundaryMarker)
			continue;

		if (line == finalBoundaryMarker)
			break;

		// read multipart headers
		multipartHeaders.clear();
		while (std::getline(ifs, line)) {
			if (!line.empty() && line.back() == '\r')
				line.pop_back();

			if (line.empty()) {
				break;
			}

			std::pair<std::string, std::string> headerField;
			if (!Utils::splitHeaderLine(line, headerField)) {
				LOGT(Log::ERROR, "Failed to split multipart/form-data headers: \"" << line << '"');
				request.errorStatusCode = WSSC_BAD_REQUEST;
				request.parsingState = Request::ParsingState::INVALID;
				return;
			}

			multipartHeaders[headerField.first] = headerField.second;
		}

		// Look for Content-Disposition with filename
		auto it = multipartHeaders.find("content-disposition");
		if (it == multipartHeaders.end()) {
			LOGT(Log::ERROR, "failed to find \"content-disposition\" in multipart/form-data headers");
			request.errorStatusCode = WSSC_BAD_REQUEST;
			request.parsingState = Request::ParsingState::INVALID;
			return;
		}

		// found content-disposition
		LOGT(Log::SUCCESS, "found: " << BOLD << LIGHTGREEN << it->first << REGULAR << GREEN << " in the multipart/form-data header, parsing parameters(filename)" << DEFAULT);
		LOGT(Log::INFO, it->first << ": " << it->second);

		const std::string& disposition = it->second;
		size_t posFilname = disposition.find("filename=");
		if (disposition.find("form-data;") == std::string::npos || posFilname == std::string::npos) {
			// invalid content-disposition
			LOGT(Log::ERROR, "Invalid content-dispositions parameters: \"" << disposition << '"');
			request.errorStatusCode = WSSC_BAD_REQUEST;
			request.parsingState = Request::ParsingState::INVALID;
			return;
		}

		std::string filename = disposition.substr(posFilname + 9);
		Utils::unquote(filename, '"');

		if (filename.empty()) {
			// return WSSC_NO_CONTENT, if the filename is empty
			request.errorStatusCode = WSSC_NO_CONTENT;
			request.parsingState = Request::ParsingState::INVALID;
			return;
		}

		static size_t count;
		std::ostringstream oss;
		oss << filename.c_str() << '.' << std::setw(20) << std::setfill('0') << count++;
		filename = oss.str();
		LOGT(Log::INFO, "final tmp filename: " << filename);


		std::string tmpPath = "./tmp/" + filename;
		std::filesystem::create_directories("./tmp/");
		std::ofstream ofs(tmpPath, std::ios::binary);
		if (!ofs) {
			LOGT(Log::ERROR, "Failed to open temporary file to store the multipart/form-data file with filename: \"" << filename << '"');
			request.errorStatusCode = WSSC_INTERNAL_SERVER_ERROR;
			request.parsingState = Request::ParsingState::INVALID;
			return;
		}
		request.tmpUploadedFiles.emplace_back(tmpPath);

		// copy from bodyFile to tmp filename
		if (!streamMultipartPartToFile(ifs, ofs, boundary)) {
			LOGT(Log::ERROR, "Failed to stream multipartpart file from tmpBody to file. uploded filename: \"" << filename << '"');
			request.errorStatusCode = WSSC_BAD_REQUEST;
			request.parsingState = Request::ParsingState::INVALID;
			return;
		}

		LOGT(Log::SUCCESS, "filename: " << LIGHTGREEN << BOLD << filename);
	}
}

bool	RequestParser::streamMultipartPartToFile(std::ifstream& ifs, std::ofstream& ofs, const std::string& boundary)
{
	const std::string boundaryPrefix = "\r\n--" + boundary;
	const std::string finalBoundaryPrefix = boundaryPrefix + "--";
	const size_t chunkSize = 8192;	//	TODO:	-> make this a define

	std::string buffer;
	std::vector<char> chunk(chunkSize);

	while (ifs) {
		ifs.read(chunk.data(), chunkSize);
		std::streamsize bytesRead = ifs.gcount();
		if (bytesRead <= 0)
			break;

		buffer.append(chunk.data(), bytesRead);

		// try to find a boundary in the buffer
		size_t pos = buffer.find(boundaryPrefix);
		if (pos == std::string::npos && buffer.rfind("--" + boundary, 0) == 0)
			pos = 0;
		if (pos == std::string::npos)
			pos = buffer.find(finalBoundaryPrefix);

		if (pos != std::string::npos) {
			// boundary found: write up to the boundary
			ofs.write(buffer.data(), pos);

			//rewind, so the next iteration can handle the boundary
			std::streamoff rewind = buffer.size() - pos;
			ifs.seekg(-rewind, std::ios::cur);
			return true;
		}

		// No boundary found: write everything except the last few bytes
		// to leave room for a possible split boundary in the next chunk
		size_t keep = boundaryPrefix.size() + 4;
		if (buffer.size() > keep) {
			ofs.write(buffer.data(), buffer.size() - keep);
			buffer.erase(0, buffer.size() - keep);
		}
	}

	// no boundary found, something is very wrong
	return false;
}
