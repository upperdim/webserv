#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "RequestHandler.hpp"
#include "Response.hpp"

#define CGI_TIMEOUT_SECONDS		5

class CGIHandler : RequestHandler
{
public:
	static void	initCgi(Request& request, Response& response);
	static bool	checkCgiCompletion(Request& request, Response& response);
	static bool	validateCgiOutput(const Request& request);
	static void	createCgiResponse(Request& request, Response& response);

private:
	CGIHandler();

	static void	failWithErrorResponse(Request& request, Response& response, int errorStatusCode);
};

#endif
