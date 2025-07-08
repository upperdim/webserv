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
	static bool			isDirectory(const std::string& path);
	static bool			fileExists(const std::string& path);
	static bool			hasPermission(const std::string& path, int mode);
	static bool			startsWith(const std::string& str, const std::string& prefix);
	static bool			startsWithHttp(const std::string& str);
	static void			trimWhitespaces(std::string& str);
	static std::string	charToHex(char c);
	static std::string	percentEncode(const std::string& str);
	static std::string	encodePath(const std::string& path);
	static void			unquote(std::string& str, char quote);

	static bool			removeDotSegments(std::string& uri);
	static bool			collapseDuplicateSlashes(std::string& oBuf);

private:
	static void			popLastSegment(std::string& oBuf);

};

#endif
