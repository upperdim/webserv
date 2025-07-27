#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include "Request.hpp"
#include "Response.hpp"

class RequestHandler
{	
public:
	static void	handle(Request& request, Response& response);

protected:
	static bool			isAllowedMethod(const Request& request);
	static std::string	getIndexAppendedResource(const Request& request);
	static void			createErrorResponse(const Request& request, Response& response, int statusCode);
	static bool			redirectOnMissingTrailingSlasch(const Request& request, Response& response);
	static void			handleAutoIndex(const Request& request, Response& response);

private:
	static const std::string	getDirListingPadding(size_t entrySize);

};

#endif
