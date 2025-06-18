#include "Utils.hpp"

std::string	Utils::sanitizePath(const Request& request, Response& response, const ServerBlock serverBlock)
{
	std::string	root_path	= serverBlock.root; //getRootDir()
	std::string path		= request.getRequestTarget();

	(void)response;	// TODO: delete me

	if (path == "/")
		path += "index.html";
	while (startsWith(path, "../"))
		path.erase(0, 3);
	while (startsWith(path, "/"))
		path.erase(0, 1);
	return (path);
}

bool	Utils::resourceExist(const std::string& path)
{
	std::ifstream	file(path);
	return ( file.good() );
}
