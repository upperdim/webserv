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
