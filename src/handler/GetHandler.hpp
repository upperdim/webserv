#ifndef GETHANDLER_HPP
#define GETHANDLER_HPP

#include "RequestHandler.hpp"

class GetHandler : RequestHandler
{
public:
	static void					handle(const Request& request, Response& response);

private:
	static std::string			getIndexAppendedResource(const Request& request);
	static void					handleAutoIndex(const Request& request, Response& response);
	static const std::string	getDirListingPadding(size_t entrySize);
};

#endif
