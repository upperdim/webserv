#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include "Request.hpp"
#include "Response.hpp"
#include "Config.hpp"

class Utils
{
public:

	static std::string	sanitizePath(const Request& request, Response& response, const ServerBlock serverBlock);
	static bool			resourceExist(const std::string& path);
};

#endif
