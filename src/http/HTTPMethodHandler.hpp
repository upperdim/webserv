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

	static void	handleGetRequest(const Request& request, Response& response);
	static void	handlePostRequest(const Request& request, Response& response);
	static void	handleDeleteRequest(const Request& request, Response& response);
	static void	handleFailedRequest(const Request& request, Response& response);

	static void	createErrorResponse(Response& response, int statusCode);

};

#endif
