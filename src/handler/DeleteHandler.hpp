#ifndef DELETEHANDLER_HPP
#define DELETEHANDLER_HPP

#include "RequestHandler.hpp"

class DeleteHandler : RequestHandler
{
public:
	static void	handleDeleteRequest(const Request& request, Response& response);
};

#endif
