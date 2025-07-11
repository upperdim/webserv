#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "RequestHandler.hpp"

#define CGI_OUTPUT_BUFFER_SIZE 4096

class CGIHandler : RequestHandler
{
public:
	static void	handle(const Request& request, Response& response);
};

#endif
