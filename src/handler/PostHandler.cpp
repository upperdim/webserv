#include "PostHandler.hpp"

void	PostHandler::handle(const Request& request, Response& response)
{
	LOGC("REQUEST_HANDLER", "-> handle POST Request", LIGHTMAGENTA, LIGHTCYAN);
	(void) request;
	(void) response;
	throw std::runtime_error("Work in progress...");
}
