#include "PostHandler.hpp"

void	PostHandler::handlePostRequest(const Request& request, Response& response)
{
	LOGC("HTTP_METHOD_HANDLER", "-> handle POST Request", LIGHTMAGENTA, LIGHTCYAN);
	(void) request;
	(void) response;
	throw std::runtime_error("Work in progress...");
}
