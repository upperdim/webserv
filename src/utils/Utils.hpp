#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include "Request.hpp"
#include "Response.hpp"
#include "Config.hpp"

class Utils
{
public:

	static std::string	sanitizePath(const Request& request, const ServerBlock serverBlock);
	static bool			resourceExist(const std::string& path);
	static bool			startsWith(const std::string& str, const std::string& prefix);
	static bool			isAllowedMethod(HTTP::Method method, const std::vector<HTTP::Method> allowdMethods);
};

#endif
