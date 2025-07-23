#ifndef POSTHANDLER_HPP
#define POSTHANDLER_HPP

#include "RequestHandler.hpp"

class PostHandler : RequestHandler
{
public:
	static void	handle(const Request& request, Response& response);

private:
	PostHandler();
};

#endif
