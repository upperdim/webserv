#include "Request.hpp"
#include <unistd.h> // close()
#include <fcntl.h>  // open()
#include <ctime>  // time()
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

Request::~Request()
{
	if (bodyFile.is_open()) {
		bodyFile.close();
	}

	if (!bodyTempFilename.empty()) {
		deleteTempBodyFile();
	}

	if (cgiOutputFile.is_open()) {
		cgiOutputFile.close();
	}

	if (!cgiOutputTempFilename.empty()) {
		deleteTempCGIOutputFile();
	}
}

void	Request::invalidateWithError(int errorStatusCode)
{
	this->errorStatusCode = errorStatusCode;
	parsingState = Request::ParsingState::INVALID;
}

bool	Request::isCGIRequest()
{
	return resolvedLocationBlock != nullptr
	       && !resolvedLocationBlock->cgiExtension.empty() 
	       && Utils::strEndsWith(URI, resolvedLocationBlock->cgiExtension);
}

bool	Request::isRedirectRequest()
{
	return resolvedLocationBlock != nullptr 
	       && !resolvedLocationBlock->returnRoute.empty()
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

// TODO: Generalized function for temporary body and cgiOutput files
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
		LOGT(Log::ERROR, "open() failed on createTempBodyFile()");
		return false;
	}
	close(tmpFileFd); // We will open it with ofstream

	// TODO: Does this need closing? Is this open() syscall?
	// TODO: Move these to separate openBodyFile closeBodyFile functions for clarity and readability?
	bodyFile.open(bodyTempFilename, std::ios::binary);
	if (!bodyFile.is_open()) {
		LOGT(Log::ERROR, "open() failed on createTempBodyFile() ofstream");
		return false;
	}

	LOGT(Log::DEBUG, "Created temp file " << bodyTempFilename);
	return true;
}

bool	Request::deleteTempBodyFile()
{
	if (bodyTempFilename.empty()) {
		LOGT(Log::INFO, "Attempted to delete non-existent temp body file ");
		return true; // Already non-existent
	}

	if (std::remove(bodyTempFilename.c_str()) != 0) {
		LOGT(Log::ERROR, "Failed to delete temp body file " << bodyTempFilename);
		return false;
	}
	
	LOGT(Log::DEBUG, "Deleted temp body file " << bodyTempFilename);
	bodyTempFilename.clear(); // Marking as deleted

	return true;
}

bool	Request::createTempCGIOutputFile()
{
	static int negativeCounter = -1;

	std::time_t now = std::time(nullptr);

	std::ostringstream oss;
	oss << "./tmp/webserv_CGI_output_" << now << "_" << negativeCounter--;
	cgiOutputTempFilename = oss.str();

	// 0600 = owner -> read write, group -> ---, others -> ---
	int tmpFileFd = open(cgiOutputTempFilename.c_str(), O_RDWR | O_CREAT | O_EXCL, 0600);
	if (tmpFileFd == -1) {
		LOGT(Log::ERROR, "open() failed on createTempCGIOutputFile()");
		return false;
	}
	close(tmpFileFd); // We will open it with ofstream

	// TODO: Does this need closing? Is this open() syscall?
	// TODO: Move these to separate openCgiOutputFile closeCgiOutputFile functions for clarity and readability?
	cgiOutputFile.open(cgiOutputTempFilename, std::ios::binary);
	if (!cgiOutputFile.is_open()) {
		LOGT(Log::ERROR, "open() failed on createTempCGIOutputFile() ofstream");
		return false;
	}

	LOGT(Log::DEBUG, "Created temp CGI output file " << cgiOutputTempFilename);
	return true;
}

bool	Request::deleteTempCGIOutputFile()
{
	if (cgiOutputTempFilename.empty()) {
		LOGT(Log::INFO, "Attempted to delete non-existent temp CGI output file ");
		return true; // Already non-existent
	}

	if (std::remove(cgiOutputTempFilename.c_str()) != 0) {
		LOGT(Log::ERROR, "Failed to delete temp CGI output file " << cgiOutputTempFilename);
		return false;
	}
	
	LOGT(Log::DEBUG, "Deleted temp CGI output file " << cgiOutputTempFilename);
	cgiOutputTempFilename.clear(); // Marking as deleted

	return true;
}
