#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <unordered_map>
#include <optional>
#include <fstream>
#include <string>
#include "HTTP.hpp"
#include "Config.hpp"

// Store request body in a temp file if it is bigger than this many bytes
#define REQUEST_BODY_SIZE_FILE_STORAGE_TRESHOLD	(10 * 1024)

class Request
{
public:
	Request(std::vector<ServerBlock>& _serverBlocks);

	enum class ParsingState {
		REQUEST_LINE,
		HEADERS,
		BODY,
		FORM_DATA,
		COMPLETE,
		INVALID
	};

	std::string										rawRequest;
	ParsingState									parsingState;
	bool											doneReceiving;
	
	// Raw request attributes
	HTTP::Method									method;
	std::optional<int>								errorStatusCode;
	std::string										requestTarget;
	std::string										protokoll;
	std::unordered_map<std::string, std::string>	headers;
	// Body
	std::ofstream									bodyFile;
	std::string										bodyTempFilename;
	std::string										currentUploadFileName;
	size_t											bodyBytesReceived;
	// Chunked transfer
	bool											isChunkedBodyTransfer;
	size_t											currentChunkSize;
	size_t											currentChunkBytesReceived;
	bool											awaitingChunkSize;

	std::vector<ServerBlock>& 						serverBlocks;

	// Matched and resolved attributes
	std::string										URI; // Decoded and sanatized requesttarget
	ServerBlock*									resolvedServerBlock;
	LocationBlock*									resolvedLocationBlock;
	std::optional<size_t>							contentLength;
	std::optional<HTTP::ContentTypeInfo_t>			contentType;
	std::string										resolvedPath;

private:
};

#endif
