#include <algorithm>
#include "RequestHandler.hpp"
#include "DeleteHandler.hpp"
#include "PostHandler.hpp"
#include "GetHandler.hpp"
#include "CGIHandler.hpp"
#include "Utils.hpp"
#include "Log.hpp"

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
	
	if (isRedirectRequest(request)) {
		response.setStatusCode(WSSC_FOUND);
		response.addHeader("Location", request.resolvedLocationBlock->returnRoute);
		response.addHeader("content-length", "0");
		return;
	}

	if (isCGIRequest(request)) {
		CGIHandler::handle(request, response);
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

bool	RequestHandler::isCGIRequest(const Request& request)
{
	return !request.resolvedLocationBlock->cgiExtension.empty() 
	       && Utils::strEndsWith(request.URI, request.resolvedLocationBlock->cgiExtension);
}

bool	RequestHandler::isRedirectRequest(const Request& request)
{
	return !request.resolvedLocationBlock->returnRoute.empty();
}
