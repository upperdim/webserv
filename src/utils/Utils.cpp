#include "Utils.hpp"
#include <iomanip>

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

void	Utils::trimWhitespaces(std::string& str)
{
	size_t	start	= 0;
	size_t	end		= str.length();
	start = str.find_first_not_of("\t\n\v\f\r ");
	if (start == std::string::npos)
	{
		str.clear();
		return ;
	}
	end = str.find_last_not_of("\t\n\v\f\r ");
	str = str.substr(start, end - start + 1);
}

bool	Utils::isAllowedMethod(HTTP::Method method, const std::vector<HTTP::Method> allowedMethods)
{
	return std::find(allowedMethods.begin(), allowedMethods.end(), method) != allowedMethods.end();
}

std::string	Utils::charToHex(char c)
{
	std::ostringstream os;
	os << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
	return os.str();
}
