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
		isCgiRequest(false),
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
	LOGT(Log::DEBUG, "Request destructor");

	if (bodyFile.is_open()) {
		bodyFile.close();
	}

	if (cgiOutFile.is_open()) {
		cgiOutFile.close();
	}

	if (!bodyFilename.empty()) {
		deleteBodyFile();
	}

	if (!cgiOutFilename.empty()) {
		deleteCgiOutFile();
	}

	if (tmpUploadedFiles.size() > 0) {
		deleteTmpUploadedFiles();
	}
}

void	Request::invalidateWithError(int errorStatusCode)
{
	this->errorStatusCode = errorStatusCode;
	parsingState = Request::ParsingState::INVALID;
}

bool	Request::isRedirectRequest() const
{
	return resolvedLocationBlock != nullptr 
	       && !resolvedLocationBlock->returnRoute.empty();
}

bool	Request::hasBody() const
{
	return contentLength.has_value() || isChunkedBodyTransfer;
}

bool	Request::isFileUploadRequest() const
{
	return contentType.has_value()
	       && contentType.value().type == HTTP::ContentType::MULTIPART_FORM_DATA;
}

std::string	Request::createFileName(std::string fileNamePrefixPath)
{
	static int counter = 0;

	std::time_t now = std::time(nullptr);

	std::ostringstream oss;
	oss << fileNamePrefixPath << now << "_" << counter++;

	return oss.str();
}

bool	Request::createFile(std::string filenamePrefix, std::string& filename)
{
	filename = createFileName(filenamePrefix);
	
	// 0600 = owner -> read write, group -> ---, others -> ---
	int tmpFileFd = open(filename.c_str(), O_RDWR | O_CREAT | O_EXCL, 0600);
	if (tmpFileFd == -1) {
		LOGT(Log::ERROR, "Failed opening " << filename);
		return false;
	}
	close(tmpFileFd); // We will open it with ofstream

	LOGT(Log::DEBUG, "Created file " << filename);
	return true;
}

bool	Request::openFile(std::ofstream& file, std::string filename)
{
	file.open(filename, std::ios::binary);
	if (!file.is_open()) {
		LOGT(Log::ERROR, "Failed to open ofstream " << filename);
		return false;
	}
	return true;
}

bool	Request::deleteFile(std::string fileName)
{
	if (fileName.empty()) {
		LOGT(Log::INFO, "Attempted to delete non-existent file ");
		return true; // Already non-existent
	}

	std::filesystem::path path(fileName.c_str());
	if (!std::filesystem::remove(path) != 0) {
		LOGT(Log::ERROR, "Failed to delete file " << fileName);
		return false;
	}
	
	LOGT(Log::DEBUG, "Deleted file " << fileName);
	fileName.clear(); // Marking as deleted

	return true;
}

bool	Request::deleteTmpUploadedFiles()
{
	bool succeeded = true; 
	for (auto it = tmpUploadedFiles.begin(); it < tmpUploadedFiles.end(); ++it) {
		if (!std::filesystem::remove(*it)) {
			LOGT(Log::ERROR, "failed to delete tmp uploaded file: " << *it);
			succeeded = false;
		}
	}
	return succeeded;
}

bool	Request::createBodyFile() { return createFile("./tmp/webserv_body_", bodyFilename); }
bool	Request::openBodyFile()   { return openFile(bodyFile, bodyFilename); }
bool	Request::deleteBodyFile() { return deleteFile(bodyFilename); }

bool	Request::createCgiOutFile() { return createFile("./tmp/webserv_cgi_output_", cgiOutFilename); }
bool	Request::openCgiOutFile()   { return openFile(cgiOutFile, cgiOutFilename); }
bool	Request::deleteCgiOutFile() { return deleteFile(cgiOutFilename); }
