#include <filesystem>
#include <algorithm>
#include <unistd.h>	  // R_OK
#include "GetHandler.hpp"
#include "Utils.hpp"

void	GetHandler::handle(const Request& request, Response& response)
{
	LOGC("REQUEST_HANDLER", "-> handle GET Request", LIGHTMAGENTA, LIGHTCYAN);

	if (Utils::isDirectory(request.resolvedPath)) {
		if (redirectOnMissingTrailingSlasch(request, response))
			return;

		if (!Utils::hasPermission(request.resolvedPath, R_OK)) {
			createErrorResponse(request, response, WSSC_FORBIDDEN);
			return;
		}

		// 1. do we have index files we could serve?
		std::string indexedResource = getIndexAppendedResource(request);
		if (Utils::fileExists(indexedResource)) {
			// check permission to serve the index
			if (!Utils::hasPermission(indexedResource, R_OK)) {
				createErrorResponse(request, response, WSSC_FORBIDDEN);
				return;
			}

			// we have permission and we serv the index
			response.setStatusCode(WSSC_OK);
			response.addHeader("Content-Type", HTTP::getMimeType(indexedResource));
			response.setBodyFileBufferReader(indexedResource);
			return;
		}

		// 2. is autoIndex enabled
		if (request.resolvedLocationBlock->autoIndex) {
			// handle autoIndex
			handleAutoIndex(request, response);
			return;
		}

		// we dont have indexedResource and no autoIndex -> Forbidden
		createErrorResponse(request, response, WSSC_FORBIDDEN);
		return;
	}

	// FILE
	size_t lastSlashPos = request.resolvedPath.find_last_of('/');
	std::string resourceDir = request.resolvedPath.substr(0, lastSlashPos);
	LOGT(Log::INFO, "resourceDir: " << resourceDir);

	// does the directory of the file exist and do we have permissions for the directory
	if (!Utils::isDirectory(resourceDir)) {
		createErrorResponse(request, response, WSSC_NOT_FOUND);
		return;
	}

	if (!Utils::hasPermission(resourceDir, R_OK)) {
		createErrorResponse(request, response, WSSC_FORBIDDEN);
		return;
	}

	// does the resource exist and do we have permissons
	if (!Utils::fileExists(request.resolvedPath)) {
		createErrorResponse(request, response, WSSC_NOT_FOUND);
		return;
	}

	if (!Utils::hasPermission(request.resolvedPath, R_OK)) {
		createErrorResponse(request, response, WSSC_FORBIDDEN);
		return;
	}
	// fetch content
	response.setStatusCode(WSSC_OK);
	response.addHeader("Content-Type", HTTP::getMimeType(request.resolvedPath));
	response.setBodyFileBufferReader(request.resolvedPath);
}

std::string	GetHandler::getIndexAppendedResource(const Request& request)
{
	std::string indexedResource = request.resolvedPath;
	if (indexedResource.back() == '/') {
		indexedResource += request.resolvedLocationBlock->index;
	} else if (!Utils::fileExists(indexedResource)) {
		indexedResource += "/" + request.resolvedLocationBlock->index;
	}
	LOGT(Log::INFO, LIGHTMAGENTA << BOLD << "indexedResource: " << LIGHTGREEN << indexedResource);
	return indexedResource;
}


void	GetHandler::handleAutoIndex(const Request& request, Response& response)
{
	LOGT(Log::INFO, LIGHTMAGENTA << "handle Auto Index");

	std::filesystem::path dirPath(request.resolvedPath);

	// sort directories first
	std::vector<std::filesystem::directory_entry> entries;

	// first store all entries
	for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
		entries.emplace_back(entry);
	}
	
	// now we sort
	std::sort(entries.begin(), entries.end(),
		[](const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b){
			if (a.is_directory() != b.is_directory())
				return a.is_directory();
			return a.path().filename().string() < b.path().filename().string();
		});

	// prepering our output stream	
	std::ostringstream os;
	os	<< "<html><head><title>" << request.resolvedLocationBlock->route
		<< "</title></head><body><h1>Index of " << request.URI
		<< "</h1><hr><pre>"
		<< "<a href=\"../\">../</a>\n";

	for (auto const& entry : entries) {

		std::string e = entry.path();

		// remove root path
		e = e.substr(request.resolvedPath.length());

		if (e.front() == '/')
			e = e.substr(1);
		if (entry.is_directory())
			e += '/';

		// trimm e to 50 characters
		if (e.length() > 50) {
			e.resize(47);
			e += "..>";
		}

		std::string anchor;
		anchor += "<a href=\"" + Utils::encodePath(e) + "\">" + e + "</a>";

		os << anchor;

		auto ftime = std::filesystem::last_write_time(entry);
		auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
		std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);
		std::tm tm = *std::gmtime(&cftime);

		os << getDirListingPadding(e.size());
		os << std::put_time(&tm, "%d-%b-%Y %H:%M");

		os << "  " << std::setw(18) << std::setfill(' ') << std::right;
		if (!entry.is_directory()) {
			try {
				os << std::to_string(entry.file_size());
			} catch(...) {
				os << "unavailable";
			} 
		}
		else
			os << "-";

		os << '\n';
	}
	os << "</pre><hr></body></html>";

	response.setStatusCode(WSSC_OK);
	response.addHeader("Content-Type", "text/html; charset=utf-8");
	response.setBodyString(os.str());
}

const std::string	GetHandler::getDirListingPadding(size_t entrySize)
{
	size_t padSize = 51;
	size_t count = 0;

	if (entrySize == 0)
		count = padSize;
	else if (entrySize >= padSize)
		count = 0;
	else
		count = padSize - entrySize;
	
	return std::string(count, ' ');
}
