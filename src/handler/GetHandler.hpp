#ifndef GETHANDLER_HPP
#define GETHANDLER_HPP

#include "RequestHandler.hpp"

class GetHandler : RequestHandler
{
public:
	static void					handle(const Request& request, Response& response);

private:
	GetHandler();

};

#endif
