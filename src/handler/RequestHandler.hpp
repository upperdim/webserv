#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include "Request.hpp"
#include "Response.hpp"

class RequestHandler
{	
public:
	static void	handle(Request& request, Response& response);
	static bool	isCGIRequest(const Request& request);

protected:
	static bool	isAllowedMethod(const Request& request);
	static void	createErrorResponse(const Request& request, Response& response, int statusCode);
	static bool	redirectOnMissingTrailingSlasch(const Request& request, Response& response);

private:
	static bool	isRedirectRequest(const Request& request);
};

#endif
