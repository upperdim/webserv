#include <filesystem>
#include <algorithm>
#include <unistd.h>	// R_OK
#include "RequestHandler.hpp"
#include "DeleteHandler.hpp"
#include "PostHandler.hpp"
#include "GetHandler.hpp"
#include "CGIHandler.hpp"
#include "Utils.hpp"
#include "Log.hpp"

void	RequestHandler::handle(Request& request, Response& response)
{
	LOGT(Log::DEBUG, "REQUEST_HANDLER");

	if (request.errorStatusCode.has_value()) {
		createErrorResponse(request, response, request.errorStatusCode.value());
		return;
	}
	
	if (request.URI.empty()) {
		createErrorResponse(request, response, WSSC_NOT_FOUND);
		return;
	}
	
	if (!isAllowedMethod(request)) {
		createErrorResponse(request, response, WSSC_FORBIDDEN);
		return;
	}
	
	if (request.isRedirectRequest()) {
		response.setStatusCode(WSSC_FOUND);
		response.addHeader("Location", request.resolvedLocationBlock->returnRoute);
		response.addHeader("content-length", "0");
		return;
	}

	if (request.isCGIRequest()) {
		CGIHandler::initCgi(request, response);
		return;
	}
	
	switch (request.method) {
		case HTTP::Method::GET:
			GetHandler::handle(request, response);
			break;
		case HTTP::Method::POST:
			PostHandler::handle(request, response);
			break;
		case HTTP::Method::DELETE:
			DeleteHandler::handle(request, response);
			break;
	}
}


//=============================================================================
// Protected
//=============================================================================

bool	RequestHandler::isAllowedMethod(const Request& request)
{
	if (request.resolvedLocationBlock == nullptr) {
		throw std::runtime_error("isAllowedmethod() with unresolved locationBlock");
	}

	return std::find(
			request.resolvedLocationBlock->allowMethods.begin(),
			request.resolvedLocationBlock->allowMethods.end(),
			request.method
		) != request.resolvedLocationBlock->allowMethods.end();
}

std::string	RequestHandler::getIndexAppendedResource(const Request& request)
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

void	RequestHandler::createErrorResponse(const Request& request, Response& response, int statusCode)
{
	if (request.resolvedLocationBlock != nullptr) {
		auto itEP = request.resolvedLocationBlock->errorPagePaths.find(statusCode);
		if (itEP != request.resolvedLocationBlock->errorPagePaths.end()
			&& !Utils::isDirectory(itEP->second)
			&& Utils::fileExists(itEP->second)
			&& Utils::hasPermission(itEP->second, R_OK)) {
			
			response.setStatusCode(statusCode);

			// get extension
			std::string ext;
			size_t pos = itEP->second.find_last_of(".");
			if (pos != std::string::npos) {
				ext = itEP->second.substr(pos);
			}

			response.addHeader("Content-Type", HTTP::getMimeType(ext));
			response.setBodyFileBufferReader(itEP->second);
			return;
		}
	}

	response.setStatusCode(statusCode);
	response.addHeader("Content-Type", HTTP::getMimeType(".html"));
	response.setBodyString(HTTP::getErrorPageTemplate(statusCode));
}

bool	RequestHandler::redirectOnMissingTrailingSlasch(const Request& request, Response& response)
{
	if (request.resolvedPath.back() != '/') {
		// If requested resource is a directory but looks like a file,
		// we redirect to another URI
		response.setStatusCode(WSSC_MOVED_PERMANENTLY);
		response.addHeader("Location", std::string(request.URI) + '/');
		response.addHeader("content-length", "0");
		return true;
	}
	return false;
}

void	RequestHandler::handleAutoIndex(const Request& request, Response& response)
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


//=============================================================================
// Private
//=============================================================================

const std::string	RequestHandler::getDirListingPadding(size_t entrySize)
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
