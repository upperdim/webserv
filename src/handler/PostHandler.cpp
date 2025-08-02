#include <unistd.h>	// W_OK
#include <filesystem>
#include <vector>
#include <algorithm>
#include "PostHandler.hpp"
#include "Utils.hpp"

void	PostHandler::handle(Request& request, Response& response)
{
	LOGC("POST_HANDLER", "-> handle POST Request", LIGHTMAGENTA, LIGHTCYAN);

	if (!Utils::isDirectory(request.resolvedPath)) {
		LOGT(Log::WARNING, "request.resolvedPath failed to be a directory: " << request.resolvedPath);
		createErrorResponse(request, response, WSSC_METHOD_NOT_ALLOWED);
		return;
	}

	if (redirectOnMissingTrailingSlasch(request, response))
		return;

	if (!Utils::hasPermission(request.resolvedPath, W_OK)) {
		createErrorResponse(request, response, WSSC_FORBIDDEN);
		return;
	}

	std::string indexedResource = getIndexAppendedResource(request);

	if (request.tmpUploadedFiles.empty()) {
		if (Utils::fileExists(indexedResource)) {
			// check permission to serve the index
			if (!Utils::hasPermission(indexedResource, R_OK)) {
				createErrorResponse(request, response, WSSC_FORBIDDEN);
				return;
			}

			response.setStatusCode(WSSC_NO_CONTENT);
			response.addHeader("Content-Type", HTTP::getMimeType(indexedResource));
			response.setBodyFileBufferReader(indexedResource);
			return;
		}

		if (request.resolvedLocationBlock->autoIndex) {
			// handle autoIndex
			handleAutoIndex(request, response);
			return;
		}

		// we dont have indexedResource and no autoIndex -> Forbidden
		createErrorResponse(request, response, WSSC_FORBIDDEN);
		return;
	}

	// logging the files we have to move
	LOGT(Log::INFO, "moving the following " << LIGHTYELLOW << request.tmpUploadedFiles.size() << BLUE << " files to " << LIGHTYELLOW << request.resolvedPath);
	for (const auto& path : request.tmpUploadedFiles)
		LOGT(Log::INFO, "=> " << path);

	std::vector<std::string> successfullyMoved;
	successfullyMoved.reserve(request.tmpUploadedFiles.size());

	size_t movedFileCount = 0;
	for (const std::string& tmpPath : request.tmpUploadedFiles) {
		std::filesystem::path source = tmpPath;
		std::string filenameStr = source.filename().string();
		if (filenameStr.length() > 21)
			filenameStr.erase(filenameStr.length() - 21);
		std::filesystem::path dest = std::filesystem::path(request.resolvedPath) / filenameStr;

		try {
			std::filesystem::rename(source, dest);
			successfullyMoved.emplace_back(tmpPath);
			LOGT(Log::SUCCESS, "Moved file to: " << dest);
			++movedFileCount;
		} catch (const std::filesystem::filesystem_error& e) {
			if (e.code() == std::errc::cross_device_link) {
				try	{
					std::filesystem::copy_file(source, dest, std::filesystem::copy_options::overwrite_existing);
					successfullyMoved.emplace_back(tmpPath);
					std::filesystem::remove(source);
					LOGT(Log::SUCCESS, "Moved file to: " << dest);
				} catch (const std::exception& e) {
					LOGT(Log::ERROR, "Failed to move file \"" << filenameStr << "\" to dest: " << dest << ": " << e.what());
					createErrorResponse(request, response, WSSC_INTERNAL_SERVER_ERROR);
					return;
				}
			} else {
				LOGT(Log::ERROR, "Failed to move file \"" << filenameStr << "\" to dest: " << dest << ": " << e.what());
				createErrorResponse(request, response, WSSC_INTERNAL_SERVER_ERROR);
				return;
			}
		} catch(const std::exception& e) {
			LOGT(Log::ERROR, "Failed to move file \"" << filenameStr << "\" to dest: " << dest << ": " << e.what());
			createErrorResponse(request, response, WSSC_INTERNAL_SERVER_ERROR);
			return;
		}
	}

	LOGT(Log::INFO, "Successfully moved " << LIGHTGREEN << request.tmpUploadedFiles.size() << BLUE << " files.");

	// erase successfully moved files from Request class tmpUploadedFiles list
	// we don't need to delete those files if error occures somewhere
	for (const std::string& path : successfullyMoved) {
		std::vector<std::string>::iterator it = std::find(
			request.tmpUploadedFiles.begin(), request.tmpUploadedFiles.end(), path
		);
		if (it != request.tmpUploadedFiles.end()) {
			request.tmpUploadedFiles.erase(it);
		}
	}

	// return 206 Partial Content if we didn't succed moving all the files
	if (request.tmpUploadedFiles.size() > 0) {
		response.setStatusCode(WSSC_PARTIAL_CONTENT);
		response.addHeader("Content-Type", HTTP::getMimeType(".html"));
		response.setBodyString(HTTP::getPartialContentTemplate(successfullyMoved, request.tmpUploadedFiles));
		return;
	}

	if (request.resolvedLocationBlock->autoIndex) {
		// handle autoIndex
		handleAutoIndex(request, response);
		response.setStatusCode(WSSC_CREATED);
		return;
	}

	if (Utils::fileExists(indexedResource)) {
		// check permission to serve the index
		if (!Utils::hasPermission(indexedResource, R_OK)) {
			createErrorResponse(request, response, WSSC_FORBIDDEN);
			return;
		}

		// create success response
		response.setStatusCode(WSSC_CREATED);
		response.addHeader("Content-Type", HTTP::getMimeType(indexedResource));
		response.setBodyFileBufferReader(indexedResource);
		return;
	}

	createErrorResponse(request, response, WSSC_FORBIDDEN);
}
