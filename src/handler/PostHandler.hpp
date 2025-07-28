#ifndef POSTHANDLER_HPP
#define POSTHANDLER_HPP

#include "RequestHandler.hpp"

class PostHandler : RequestHandler
{
public:
	static void	handle(Request& request, Response& response);
};

#endif
