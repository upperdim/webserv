#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>
#include "HTTP.hpp"
#include "Request.hpp"
#include "Config.hpp"

class Utils
{
public:

	static std::string	sanitizePath(const Request& request, const ServerBlock serverBlock);
	static bool			resourceExist(const std::string& path);
	static bool			startsWith(const std::string& str, const std::string& prefix);
	static void			trimWhitespaces(std::string& str);
	static std::string	charToHex(char c);
};

#endif
