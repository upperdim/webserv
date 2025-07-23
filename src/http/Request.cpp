#include "Request.hpp"
#include <algorithm>
#include <fcntl.h>  // open()
#include <unistd.h> // close()
#include "Utils.hpp"
#include "Log.hpp"

Request::Request(std::vector<ServerBlock>& _serverBlocks)
	:	parsingState(ParsingState::REQUEST_LINE),
		doneReceiving(false),
		method(HTTP::Method::GET),
		errorStatusCode(std::nullopt),
		bodyBytesStored(0),
		isChunkedBodyTransfer(false),
		currentChunkSize(0),
		currentChunkBytesReceived(0),
		awaitingChunkSize(true),
		serverBlocks(_serverBlocks),
		resolvedServerBlock(nullptr),
		resolvedLocationBlock(nullptr)
{
}

bool	Request::isCGIRequest()
{
	return !resolvedLocationBlock->cgiExtension.empty() 
	       && Utils::strEndsWith(URI, resolvedLocationBlock->cgiExtension);
}

bool	Request::isRedirectRequest()
{
	return !resolvedLocationBlock->returnRoute.empty()
	       && Utils::strEndsWith(URI, resolvedLocationBlock->cgiExtension);
}

bool	Request::hasBody()
{
	return contentLength.has_value() || isChunkedBodyTransfer;
}

bool	Request::isFileUploadRequest()
{
	return contentType.has_value()
	       && contentType.value().type == HTTP::ContentType::MULTIPART_FORM_DATA;
}

bool	Request::createTempBodyFile()
{
	static int counter = 0;

	std::time_t now = std::time(nullptr);

	std::ostringstream oss;
	oss << "./tmp/webserv_body_" << now << "_" << counter++;
	bodyTempFilename = oss.str();

	// 0600 = owner -> read write, group -> ---, others -> ---
	int tmpFileFd = open(bodyTempFilename.c_str(), O_RDWR | O_CREAT | O_EXCL, 0600);
	if (tmpFileFd == -1) {
		LOGT(Log::ERROR, "open() failed");
		return false;
	}
	close(tmpFileFd); // We will open it with ofstream

	bodyFile.open(bodyTempFilename, std::ios::binary);
	if (!bodyFile.is_open()) {
		LOGT(Log::ERROR, "open() failed on ofstream");
		return false;
	}

	LOGT(Log::DEBUG, "Created temp file " << bodyTempFilename);
	return true;
}

bool	Request::deleteTempBodyFile()
{
	if (bodyTempFilename.empty()) {
		LOGT(Log::INFO, "Attempted to delete non-existent file " << bodyTempFilename);
		return true; // Already non-existent
	}

	if (std::remove(bodyTempFilename.c_str()) != 0) {
		LOGT(Log::ERROR, "Failed to delete temp file " << bodyTempFilename);
		return false;
	}
	
	LOGT(Log::DEBUG, "Deleted temp file " << bodyTempFilename);
	bodyTempFilename.clear(); // Marking as deleted

	return true;
}
