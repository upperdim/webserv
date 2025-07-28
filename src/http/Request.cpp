#include "Request.hpp"
#include <filesystem>
#include <ctime>    // time()
#include <unistd.h> // close()
#include <fcntl.h>  // open()
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
	cgiSession.pid = -1;
	cgiSession.state = CgiState::INIT;
}

Request::~Request()
{
	if (bodyFile.is_open()) {
		bodyFile.close();
	}

	if (!bodyFilename.empty()) {
		deleteBodyFile();
	}

	if (cgiOutFile.is_open()) {
		cgiOutFile.close();
	}

	if (!cgiOutFilename.empty()) {
		deleteCgiOutFile();
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

//=============================================================================
// General File Methods
//=============================================================================

std::string	Request::createFileName(std::string fileNamePrefixPath)
{
	static int counter = 0;

	std::time_t now = std::time(nullptr);

	std::ostringstream oss;
	oss << fileNamePrefixPath << now << "_" << counter++;

	return oss.str();
}

bool	Request::deleteFile(std::string fileName)
{
	if (fileName.empty()) {
		LOGT(Log::INFO, "Attempted to delete non-existent file ");
		return true; // Already non-existent
	}

	std::filesystem::path path(fileName.c_str());
	if (std::filesystem::remove(path) != 0) {
		LOGT(Log::ERROR, "Failed to delete file " << fileName);
		return false;
	}
	
	LOGT(Log::DEBUG, "Deleted file " << fileName);
	fileName.clear(); // Marking as deleted

	return true;
}

//=============================================================================
// Body File Methods
//=============================================================================

bool	Request::createBodyFile()
{
	bodyFilename = createFileName("./tmp/webserv_body_");
	
	// 0600 = owner -> read write, group -> ---, others -> ---
	int tmpFileFd = open(bodyFilename.c_str(), O_RDWR | O_CREAT | O_EXCL, 0600);
	if (tmpFileFd == -1) {
		LOGT(Log::ERROR, "open() failed on createTempBodyFile()");
		return false;
	}
	close(tmpFileFd); // We will open it with ofstream

	LOGT(Log::DEBUG, "Created temp file " << bodyFilename);
	return true;
}

bool	Request::openBodyFile()
{
	bodyFile.open(bodyFilename, std::ios::binary);
	if (!bodyFile.is_open()) {
		LOGT(Log::ERROR, "open() failed on createTempBodyFile() ofstream");
		return false;
	}
	return true;
}

bool	Request::deleteBodyFile()
{
	return deleteFile(bodyFilename);
}

//=============================================================================
// CGI Output File Methods
//=============================================================================

bool	Request::createCgiOutFile()
{
	cgiOutFilename = createFileName("./tmp/webserv_CGI_output_");

	// 0600 = owner -> read write, group -> ---, others -> ---
	int tmpFileFd = open(cgiOutFilename.c_str(), O_RDWR | O_CREAT | O_EXCL, 0600);
	if (tmpFileFd == -1) {
		LOGT(Log::ERROR, "open() failed on createTempCGIOutputFile()");
		return false;
	}
	close(tmpFileFd); // We will open it with ofstream

	LOGT(Log::DEBUG, "Created temp CGI output file " << cgiOutFilename);
	return true;
}

bool	Request::openCgiOutFile()
{
	cgiOutFile.open(cgiOutFilename, std::ios::binary);
	if (!cgiOutFile.is_open()) {
		LOGT(Log::ERROR, "open() failed on createTempCGIOutputFile() ofstream");
		return false;
	}
	return true;
}

bool	Request::deleteCgiOutFile()
{
	return deleteFile(cgiOutFilename);
}
