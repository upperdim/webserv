#include <unistd.h>	// R_OK
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
		createErrorResponse(request, response, request.errorStatusCode.value());
		return;
	}
	
	if (request.URI.empty()) {
		createErrorResponse(request, response, WSSC_NOT_FOUND);
		return;
	}
	
	if (!isAllowedMethod(request)) {
		createErrorResponse(request, response, WSSC_FORBIDDEN);
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

void	RequestHandler::createErrorResponse(const Request& request, Response& response, int statusCode)
{
	auto itEP = request.resolvedLocationBlock->errorPagePaths.find(statusCode);
	if (itEP != request.resolvedLocationBlock->errorPagePaths.end()
	    && !Utils::isDirectory(itEP->second)
		&& Utils::fileExists(itEP->second)
		&& Utils::hasPermission(itEP->second, R_OK)) {
		response.setStatusCode(statusCode);

		// get extension
		std::string ext;
		size_t pos = itEP->second.find_last_of(".");
		if (pos != std::string::npos) {
			ext = itEP->second.substr(pos);
		}

		response.addHeader("Content-Type", HTTP::getMimeType(ext));
		response.setBodyFileBufferReader(itEP->second);
		return;
	}

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
	return !request.resolvedLocationBlock->returnRoute.empty()
	       && Utils::strEndsWith(request.URI, request.resolvedLocationBlock->cgiExtension);
}
