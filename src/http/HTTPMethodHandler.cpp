#include <filesystem>
#include "HTTPMethodHandler.hpp"
#include "Utils.hpp"
#include "unistd.h"	// for R_OK

HTTPMethodHandler::~HTTPMethodHandler()
{
}

/* ************************************************************************** */
/* ************************************************************************** */


void	HTTPMethodHandler::handle(Request& request, Response& response)
{
	if (request.errorStatusCode.has_value()) {
		createErrorResponse(response, request.errorStatusCode.value());
		return;
	}
	
	if (request.URI.empty()) {
		response.setStatus(WSSC_NOT_FOUND);
		return;
	}
	
	if (!request.isAllowedMethod()) {
		response.setStatus(WSSC_METHOD_NOT_ALLOWED);
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

	// TODO: repetitive?
	if (request.errorStatusCode.has_value()) {
		createErrorResponse(response, request.errorStatusCode.value());
		return;
	}

	// do we have a file or directory
	if (Utils::isDirectory(request.resolvedPath)) {
		// =====================================================================
		// DIRECTORY
		// =====================================================================
		if (!Utils::hasPermission(request.resolvedPath, R_OK)) {
			createErrorResponse(response, WSSC_FORBIDDEN);
			return;
		}

		// 1. do we have index files we could serve?
		std::string indexedResource = indexModule(request);
		if (Utils::fileExists(indexedResource)) {
			// check permission to serve the index
			if (!Utils::hasPermission(indexedResource, R_OK)) {
				createErrorResponse(response, WSSC_FORBIDDEN);
				return;
			}

			// we have permission and we serv the index
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

	// =========================================================================
	// FILE
	// =========================================================================
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
	if (Utils::fileExists(request.resolvedPath)) {
		if (!Utils::hasPermission(request.resolvedPath, R_OK)) {
			createErrorResponse(response, WSSC_FORBIDDEN);
			return;
		}
		// fetch content
		response.addHeader("Content-Type", HTTP::getMimeType(request.resolvedPath));
		response.setBodyFileBufferReader(request.resolvedPath);
		return;
	}

	// we have not found the resource:
	createErrorResponse(response, WSSC_NOT_FOUND);
}

void	HTTPMethodHandler::handlePostRequest(const Request& request, Response& response)
{
	LOGC("HTTP_METHOD_HANDLER", "-> handle POST Request", LIGHTMAGENTA, LIGHTCYAN);
	(void) request;
	(void) response;
	throw std::runtime_error("Work in progress...");
}

void	HTTPMethodHandler::handleDeleteRequest(const Request& request, Response& response)
{
	LOGC("HTTP_METHOD_HANDLER", "-> handle DELETE Request", LIGHTMAGENTA, LIGHTCYAN);

	std::filesystem::path resourcePath(Utils::sanitizePath(request, *request.resolvedServerBlock));
	if (!(std::filesystem::exists(resourcePath) && std::filesystem::is_regular_file(resourcePath))) {
		// resourcePath NOT FOUND
		response.setStatus(WSSC_NOT_FOUND);
		return;
	}

	LOG_WARNING_LM("DELETING", resourcePath.c_str());
	if (!std::filesystem::remove(resourcePath)) {
		// failed to remove
		response.setStatus(WSSC_INTERNAL_SERVER_ERROR);
		return;
	}
	LOG_SUCCESS(std::string("deleted: ") + resourcePath.c_str());

	response.setStatus(WSSC_OK);
}

void	HTTPMethodHandler::createErrorResponse(Response& response, int statusCode)
{
	//	TODO:	reexamen this will we need it like that or can we reduce
	//			the statusCode for exameple etc…
	response.setProtokoll("HTTP/1.1");
	response.setStatus(statusCode);
	response.addHeader("Content-Type", HTTP::getMimeType(".html"));
	response.setBodyString(HTTP::getErrorPageTemplate(statusCode));
}


//=============================================================================
// Modules
//=============================================================================

std::string	HTTPMethodHandler::indexModule(const Request& request)
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


//=============================================================================
// Sections
//=============================================================================

void	HTTPMethodHandler::handleAutoIndex(const Request& request, Response& response)
{
	(void)request;
	(void)response;
	// if (Utils::isDirectory(resourcePath) && request.resolvedLocationBlock->autoIndex) {
	// 	// respond with the index of files in the directory
	// 	// TODO: implementation
	// 	LOG_DEBUG(" ----------------------------------------------> AUTO INDEX ");

		
	// 	std::ostringstream os;
	// 	os	<< "<html><head><title>" << location.route
	// 		<< "</title></head><body><h1>Index of " << request.getRequestTarget()
	// 		<< "</h1><hr><pre>"
	// 		<< "<a href=\"../\">../</a>\n";

	// 	std::filesystem::path dirPath(resourcePath), end;

	// 	for (auto const& entry : std::filesystem::directory_iterator(dirPath)) {

	// 		std::string e = entry.path();
	// 		// remove root path
	// 		e = e.substr(m_serverBlock.root.length() + request.getRequestTarget().length() - 1);
	// 		if (e.front() == '/')
	// 			e = e.substr(1);
	// 		if (entry.is_directory())
	// 			e += '/';
			
	// 		std::string url(request.getRequestTarget());
	// 		url += e;

	// 		std::string anchor;
	// 		// anchor += request.getRequestTarget();
	// 		anchor += "<a href=\"" + url + "\">" + e + "</a>";

	// 		os << anchor;

	// 		auto ftime = std::filesystem::last_write_time(entry);
	// 		auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
	// 		std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);
	// 		std::tm tm = *std::gmtime(&cftime);
			
	// 		os << Utils::getDirListingPadding(e.size());
	// 		os << std::put_time(&tm, "%d-%b-%Y %H:%M");

	// 		os << "                   ";
	// 		if (!entry.is_directory())
	// 			os << std::to_string(entry.file_size());
	// 		else
	// 			os << "-";

	// 		os << '\n';
	// 	}
	// 	os << "</pre><hr><body><html>";


	// 	response.setBodyString(os.str());
	// 	return response;
	// }
}