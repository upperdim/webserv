#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "RequestHandler.hpp"

#define CGI_OUTPUT_BUFFER_SIZE	4096

#if defined(__linux__)
	#define PYTHON3_PATH			"/usr/bin/python3"
#elif defined(__APPLE__) && defined(__MACH__)
	#define PYTHON3_PATH			"/usr/local/bin/python3"
#endif

class CGIHandler : RequestHandler
{
public:
	static void	handle(const Request& request, Response& response);
};

#endif
