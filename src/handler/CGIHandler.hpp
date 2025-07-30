#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "RequestHandler.hpp"
#include "Response.hpp"

#define CGI_OUTPUT_BUFFER_SIZE	4096
#define CGI_TIMEOUT_SECONDS		5

#if defined(__linux__)
	#define PYTHON3_PATH			"/usr/bin/python3"
#elif defined(__APPLE__) && defined(__MACH__)
	#define PYTHON3_PATH			"/usr/local/bin/python3"
#endif

class CGIHandler : RequestHandler
{
public:
	static void	initCgi(Request& request, Response& response);
	static bool	checkCgiCompletion(Request& request, Response& response);
	static bool	validateCgiOutput(const Request& request);
	static void	createCgiResponse(Request& request, Response& response);

private:
	CGIHandler();
};

#endif
