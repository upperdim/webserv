#include <filesystem>
#include <vector>
#include <algorithm>
#include "HTTPMethodHandler.hpp"
#include "Utils.hpp"
#include "unistd.h"	// for R_OK


//=============================================================================
// Public Methods
//=============================================================================

void	HTTPMethodHandler::handle(Request& request, Response& response)
{
	if (request.errorStatusCode.has_value()) {
		createErrorResponse(response, request.errorStatusCode.value());
		return;
	}
	
	if (request.URI.empty()) {
		createErrorResponse(response, WSSC_NOT_FOUND);
		return;
	}
	
	if (!request.isAllowedMethod()) {
		createErrorResponse(response, WSSC_METHOD_NOT_ALLOWED);
		return;
	}
	
	switch (request.method) {
		case HTTP::Method::GET:
			handleGetRequest(request, response);
			break;
		case HTTP::Method::POST:
			handlePostRequest(request, response);
			break;
		case HTTP::Method::DELETE:
			handleDeleteRequest(request, response);
			break;
	}

}

//=============================================================================
// Handlers
//=============================================================================

void	HTTPMethodHandler::handleGetRequest(const Request& request, Response& response)
{
	LOGC("HTTP_METHOD_HANDLER", "-> handle GET Request", LIGHTMAGENTA, LIGHTCYAN);

	if (Utils::isDirectory(request.resolvedPath)) {
		// Check trailng slash or redirect
		if (request.resolvedPath.back() != '/') {
			// If requested resource is a directory but looks like a file,
			// we redirect to another URI
			response.setStatusCode(WSSC_MOVED_PERMANENTLY);
			response.addHeader("Location", std::string(request.URI) + '/');
			response.addHeader("content-length", "0");
			return;
		}

		if (!Utils::hasPermission(request.resolvedPath, R_OK)) {
			createErrorResponse(response, WSSC_FORBIDDEN);
			return;
		}

		// 1. do we have index files we could serve?
		std::string indexedResource = getIndexAppendedResource(request);
		if (Utils::fileExists(indexedResource)) {
			// check permission to serve the index
			if (!Utils::hasPermission(indexedResource, R_OK)) {
				createErrorResponse(response, WSSC_FORBIDDEN);
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
		createErrorResponse(response, WSSC_FORBIDDEN);
		return;
	}

	// FILE
	size_t lastSlashPos = request.resolvedPath.find_last_of('/');
	std::string resourceDir = request.resolvedPath.substr(0, lastSlashPos);
	LOGT(Log::INFO, "resourceDir: " << resourceDir);

	// does the directory of the file exist and do we have permissions for the directory
	if (!Utils::isDirectory(resourceDir)) {
		createErrorResponse(response, WSSC_NOT_FOUND);
		return;
	}
	if (!Utils::hasPermission(resourceDir, R_OK)) {
		createErrorResponse(response, WSSC_FORBIDDEN);
		return;
	}

	// does the resource exist and do we have permissons
	if (!Utils::fileExists(request.resolvedPath)) {
		// we have not found the resource:
		createErrorResponse(response, WSSC_NOT_FOUND);
	}
	if (!Utils::hasPermission(request.resolvedPath, R_OK)) {
		createErrorResponse(response, WSSC_FORBIDDEN);
		return;
	}
	// fetch content
	response.setStatusCode(WSSC_OK);
	response.addHeader("Content-Type", HTTP::getMimeType(request.resolvedPath));
	response.setBodyFileBufferReader(request.resolvedPath);
}

void	HTTPMethodHandler::handlePostRequest(const Request& request, Response& response)
{
	LOGC("HTTP_METHOD_HANDLER", "-> handle POST Request", LIGHTMAGENTA, LIGHTCYAN);

	if (!Utils::isDirectory(request.resolvedPath)) {
		createErrorResponse(response, WSSC_METHOD_NOT_ALLOWED);
		return;
	}

	//	TODO:	this is repetitive, shall we create a function =)
	//			also used in handleGetRequest()
	if (request.resolvedPath.back() != '/') {
		// If requested resource is a directory but looks like a file,
		// we redirect to another URI
		response.setStatusCode(WSSC_MOVED_PERMANENTLY);
		response.addHeader("Location", std::string(request.URI) + '/');
		response.addHeader("content-length", "0");
		return;
	}

	if (!Utils::hasPermission(request.resolvedPath, W_OK)) {
		createErrorResponse(response, WSSC_FORBIDDEN);
		return;
	}

	//	////////////////////
	//	TODO:
	//	////////////////////

}

void	HTTPMethodHandler::handleDeleteRequest(const Request& request, Response& response)
{
	LOGC("HTTP_METHOD_HANDLER", "-> handle DELETE Request", LIGHTMAGENTA, LIGHTCYAN);

	std::filesystem::path resourcePath(request.resolvedPath);

	// do we have the permissions in this path
	std::filesystem::path parentPath = resourcePath.parent_path();
	if (!parentPath.empty() && Utils::isDirectory(parentPath)) {
		if (!Utils::hasPermission(parentPath, W_OK)) {
			createErrorResponse(response, WSSC_FORBIDDEN);
			return;
		}
	}

	if (Utils::isDirectory(resourcePath)) {
		createErrorResponse(response, WSSC_FORBIDDEN);
		return;
	}

	if (!Utils::fileExists(request.resolvedPath)) {
		createErrorResponse(response, WSSC_NOT_FOUND);
	}

	if (!Utils::hasPermission(request.resolvedPath, W_OK)) {
		createErrorResponse(response, WSSC_FORBIDDEN);
		return;
	}

	try {
		LOG_WARNING_LM("DELETING", resourcePath.c_str());
		if (!std::filesystem::remove(resourcePath)) {
			// failed to remove
			createErrorResponse(response, WSSC_INTERNAL_SERVER_ERROR);
			return;
		}
		LOG_SUCCESS(std::string("deleted: ") + resourcePath.c_str());

		response.setStatusCode(WSSC_OK);
		return;
	} catch(const std::exception& e) {
		LOGT(Log::ERROR, "Failed to remove file: " << e.what());
		createErrorResponse(response, WSSC_INTERNAL_SERVER_ERROR);
		return;
	}
}

void	HTTPMethodHandler::createErrorResponse(Response& response, int statusCode)
{
	//	TODO:	reexamen this will we need it like that or can we reduce
	//			the statusCode for exameple etc…
	response.setStatusCode(statusCode);
	response.addHeader("Content-Type", HTTP::getMimeType(".html"));
	response.setBodyString(HTTP::getErrorPageTemplate(statusCode));
}

std::string	HTTPMethodHandler::getIndexAppendedResource(const Request& request)
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

void	HTTPMethodHandler::handleAutoIndex(const Request& request, Response& response)
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

		os << "                   ";
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

const std::string	HTTPMethodHandler::getDirListingPadding(size_t entrySize)
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
