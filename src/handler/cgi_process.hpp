#ifndef CGI_PROCESS_HPP
#define CGI_PROCESS_HPP

#include <ctime>
#include <string>
#include "Request.hpp"
#include "Response.hpp"

struct CgiProcess
{
	pid_t		pid;
	std::string	outputFile;
	int			outputReadFd = -1;
	Response*	response = nullptr;
	Request		request;
	bool		finished = false;
	time_t		startTime;
};

#endif
