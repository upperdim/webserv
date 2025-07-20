#include <unistd.h>	// W_OK
#include "PostHandler.hpp"
#include "Utils.hpp"

void	PostHandler::handle(const Request& request, Response& response)
{
	LOGC("HTTP_METHOD_HANDLER", "-> handle POST Request", LIGHTMAGENTA, LIGHTCYAN);

	if (!Utils::isDirectory(request.resolvedPath)) {
		createErrorResponse(request, response, WSSC_METHOD_NOT_ALLOWED);
		return;
	}

	if (redirectOnMissingTrailingSlasch(request, response))
		return;

	if (!Utils::hasPermission(request.resolvedPath, W_OK)) {
		createErrorResponse(request, response, WSSC_FORBIDDEN);
		return;
	}

	//	////////////////////
	//	TODO:
	//	////////////////////
}
