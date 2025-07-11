#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "RequestHandler.hpp"

#define CGI_OUTPUT_BUFFER_SIZE	4096
#define PYTHON3_PATH			"/usr/local/bin/python3"

class CGIHandler : RequestHandler
{
public:
	static void	handle(const Request& request, Response& response);
};

#endif
