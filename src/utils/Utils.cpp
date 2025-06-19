#include "Utils.hpp"

std::string	Utils::sanitizePath(const Request& request, const ServerBlock serverBlock)
{
	std::string	root_path	= serverBlock.root;
	std::string path		= request.getRequestTarget();

	if (path == "/") {
		if (serverBlock.index.empty())
			path += "index.html";
		else
			path += serverBlock.index;
	}

	//	TODO:	do we retirn an Resons error if the User tries to move up the direcoty???
	// remove the '/' on path and all attempts to move up the director
	while (startsWith(path, "../"))
		path.erase(0, 3);
	while (startsWith(path, "/"))
		path.erase(0, 1);

	//	TODO:	Do we allow serverBlock.root to be a file or only a directory ???
	if (root_path.back() != '/')
		root_path += '/';

	return root_path + path;
}

bool	Utils::resourceExist(const std::string& path)
{
	std::ifstream	file(path);
	return ( file.good() );
}

bool	Utils::startsWith(const std::string& str, const std::string& prefix)
{
	if (str.size() < prefix.size())
		return (false);
	return ( std::equal(prefix.begin(), prefix.end(), str.begin()) );
}

bool	Utils::isAllowedMethod(HTTP::Method method, const std::vector<HTTP::Method> allowedMethods)
{
	return std::find(allowedMethods.begin(), allowedMethods.end(), method) != allowedMethods.end();
}
