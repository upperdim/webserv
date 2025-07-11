#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include "Request.hpp"
#include "Response.hpp"

class RequestHandler
{	
public:
	static void	handle(Request& request, Response& response);

protected:
	static bool	isAllowedMethod(const Request& request);
	static void	createErrorResponse(Response& response, int statusCode);

private:
	static bool	isRedirectRequest(const Request& request);
	static bool	isCGIRequest(const Request& request);
};

#endif
