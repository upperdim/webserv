#include "RequestHandler.hpp"
#include "GetHandler.hpp"
#include "PostHandler.hpp"
#include "DeleteHandler.hpp"

void	RequestHandler::handle(Request& request, Response& response)
{
	if (request.errorStatusCode.has_value()) {
		createErrorResponse(response, request.errorStatusCode.value());
		return;
	}
	
	if (request.URI.empty()) {
		createErrorResponse(response, WSSC_NOT_FOUND);
		return;
	}
	
	if (!isAllowedMethod(request)) {
		createErrorResponse(response, WSSC_FORBIDDEN);
		return;
	}
	
	if (handleIfRedirect(request, response)) {
		return;
	}
	
	switch (request.method) {
		case HTTP::Method::GET:
			GetHandler::handle(request, response);
			break;
		case HTTP::Method::POST:
			PostHandler::handle(request, response);
			break;
		case HTTP::Method::DELETE:
			DeleteHandler::handle(request, response);
			break;
	}
}

//	TODO:	we could create a method redirect(statusCode, redirectPathURI, resposne)
//			and put the if staments of  
//				-	handleIfRedirect()
//				-	redirectOnMissingTrailingSlasch()
//			into the handlers and just call the redirect() method then

bool	RequestHandler::handleIfRedirect(const Request& request, Response& response)
{
	if (!request.resolvedLocationBlock->returnRoute.empty()) {
		response.setStatusCode(WSSC_FOUND);
		response.addHeader("Location", request.resolvedLocationBlock->returnRoute);
		response.addHeader("content-length", "0");
		return true;
	}
	return false;
}

bool	RequestHandler::redirectOnMissingTrailingSlasch(const Request& request, Response& response)
{
	if (request.resolvedPath.back() != '/') {
		// If requested resource is a directory but looks like a file,
		// we redirect to another URI
		response.setStatusCode(WSSC_MOVED_PERMANENTLY);
		response.addHeader("Location", std::string(request.URI) + '/');
		response.addHeader("content-length", "0");
		return true;
	}
	return false;
}


bool	RequestHandler::isAllowedMethod(const Request& request)
{
	if (request.resolvedLocationBlock == nullptr) {
		throw std::runtime_error("isAllowedmethod() with unresolved locationBlock");
	}

	return std::find(
			request.resolvedLocationBlock->allowMethods.begin(),
			request.resolvedLocationBlock->allowMethods.end(),
			request.method
		) != request.resolvedLocationBlock->allowMethods.end();
}

void	RequestHandler::createErrorResponse(Response& response, int statusCode)
{
	//	TODO:	reexamen this will we need it like that or can we reduce
	//			the statusCode for exameple etc…
	response.setStatusCode(statusCode);
	response.addHeader("Content-Type", HTTP::getMimeType(".html"));
	response.setBodyString(HTTP::getErrorPageTemplate(statusCode));
}

