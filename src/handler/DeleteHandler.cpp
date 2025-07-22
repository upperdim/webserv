#include <filesystem>
#include <unistd.h>	  // R_OK
#include "DeleteHandler.hpp"
#include "Utils.hpp"

void	DeleteHandler::handle(const Request& request, Response& response)
{
	LOGC("REQUEST_HANDLER", "-> handle DELETE Request", LIGHTMAGENTA, LIGHTCYAN);

	std::filesystem::path resourcePath(request.resolvedPath);

	// do we have the permissions in this path
	std::filesystem::path parentPath = resourcePath.parent_path();
	if (!parentPath.empty() && Utils::isDirectory(parentPath)) {
		if (!Utils::hasPermission(parentPath, W_OK)) {
			createErrorResponse(request, response, WSSC_FORBIDDEN);
			return;
		}
	}

	if (Utils::isDirectory(resourcePath)) {
		createErrorResponse(request, response, WSSC_FORBIDDEN);
		return;
	}

	if (!Utils::fileExists(request.resolvedPath)) {
		createErrorResponse(request, response, WSSC_NOT_FOUND);
		return;
	}

	if (!Utils::hasPermission(request.resolvedPath, W_OK)) {
		createErrorResponse(request, response, WSSC_FORBIDDEN);
		return;
	}

	try {
		LOG_WARNING_LM("DELETING", resourcePath.c_str());
		if (!std::filesystem::remove(resourcePath)) {
			// failed to remove
			createErrorResponse(request, response, WSSC_INTERNAL_SERVER_ERROR);
			return;
		}
		LOG_SUCCESS(std::string("deleted: ") + resourcePath.c_str());

		response.setStatusCode(WSSC_OK);
		return;
	} catch(const std::exception& e) {
		LOGT(Log::ERROR, "Failed to remove file: " << e.what());
		createErrorResponse(request, response, WSSC_INTERNAL_SERVER_ERROR);
		return;
	}
}
