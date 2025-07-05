#ifndef HTTP_METHOD_HANDLER_HPP
#define HTTP_METHOD_HANDLER_HPP

#include "Request.hpp"
#include "Response.hpp"

class HTTPMethodHandler
{
public:
	~HTTPMethodHandler();

	static void			handle(Request& request, Response& response);

private:
	HTTPMethodHandler();

	static void					handleGetRequest(const Request& request, Response& response);
	static void					handlePostRequest(const Request& request, Response& response);
	static void					handleDeleteRequest(const Request& request, Response& response);
	static void					createErrorResponse(Response& response, int statusCode);

	static std::string			getIndexAppendedResource(const Request& request);

	static void					handleAutoIndex(const Request& request, Response& response);
	static const std::string	getDirListingPadding(size_t entrySize);

	static bool					redirectOnMissingTrailingSlasch(const Request& request, Response& response);

};

#endif
