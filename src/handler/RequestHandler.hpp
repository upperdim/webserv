#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include "Request.hpp"
#include "Response.hpp"

class RequestHandler
{	
public:
	static void	handle(Request& request, Response& response);

protected:
	static bool	handleIfRedirect(const Request& request, Response& response);
	static bool	handleIfCGI(const Request& request, Response& response);
	static bool	isAllowedMethod(const Request& request);
	static void	createErrorResponse(Response& response, int statusCode);
};

#endif
