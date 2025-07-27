#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <unordered_map>
#include <optional>
#include <fstream>
#include <string>
#include "HTTP.hpp"
#include "Config.hpp"

class Request
{
public:
	Request(std::vector<ServerBlock>& _serverBlocks);
	~Request();

	enum class ParsingState {
		REQUEST_LINE,
		HEADERS,
		BODY,
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
	// CGI Output
	std::ofstream									cgiOutFile;
	std::string										cgiOutFilename;
	// Body
	std::ofstream									bodyFile;
	std::string										bodyFilename;
	size_t											bodyBytesStored;
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
	std::string										queryString; // URL query string after '?'

	void											invalidateWithError(int errorStatusCode);
	bool											isCGIRequest();
	bool											isRedirectRequest();
	bool											hasBody();
	bool											isFileUploadRequest();
	
	std::string										createFileName(std::string fileNamePrefixPath, int countDirection);
	bool											deleteFile(std::string fileName);

	bool											createBodyFile();
	bool											openBodyFile();
	bool											deleteBodyFile();
	
	bool											createCgiOutFile();
	bool											openCgiOutFile();
	bool											deleteCgiOutFile();


private:
};

#endif
