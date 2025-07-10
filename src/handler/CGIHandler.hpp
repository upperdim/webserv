#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "RequestHandler.hpp"

#define CGI_OUTPUT_BUFFER_SIZE 4096

class CGIHandler : RequestHandler
{
public:
	static void	handle(const Request& request, Response& response);

private:
	static void	runCgiScript(const std::string& scriptPath,
	                         const HTTP::Method& method,
	                         const std::string& queryString,
	                         //  const std::string& requestBody,
	                         //  const std::string& contentType,
	                         const Request& request,
	                         Response& response);
};

#endif
