#include "Utils.hpp"
#include <iomanip>
#include <sstream>
#include <filesystem>
#include "Log.hpp"
#include "unistd.h"

std::string	Utils::sanitizePath(const Request& request, const ServerBlock serverBlock)
{
	std::string	root_path	= serverBlock.root;
	std::string path		= request.URI;

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

bool	Utils::isDirectory(const std::string& path)
{
	try {
		std::filesystem::path pathToDir(path);
		return (std::filesystem::exists(pathToDir)
				&& std::filesystem::is_directory(pathToDir));
	} catch(const std::filesystem::filesystem_error& e) {
		LOGT(Log::WARNING, e.what());
		return false;
	}
}

bool	Utils::fileExists(const std::string& path)
{
	try {
		std::filesystem::path pathToFile(path);
		return (std::filesystem::exists(pathToFile) 
		        && std::filesystem::is_regular_file(pathToFile));
	} catch(const std::filesystem::filesystem_error& e) {
		LOGT(Log::WARNING, e.what());
		return false;
	}
}

bool	Utils::hasPermission(const std::string& path, int mode)
{
	// mode can be R_OK, W_OK, X_OK, or F_OK
	return (access(path.c_str(), mode) == 0);
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


std::string	Utils::charToHex(char c)
{
	std::ostringstream os;
	os << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
	return os.str();
}

std::string	Utils::percentEncode(const std::string& str)
{
	std::ostringstream encoded;

	encoded.fill('0');		// set fill character
	encoded << std::hex;	// numbers as hex

	for (char c : str) {
		if (std::isalnum(static_cast<unsigned char>(c)) ||
			c == '-' || c == '_' || c == '.' || c == '~') {
			encoded << c;
		} else {
			encoded << '%' << std::setw(2) << std::uppercase
			        << static_cast<unsigned int>(static_cast<unsigned char>(c));
		}
	}
	return encoded.str();
}

std::string	Utils::encodePath(const std::string& path)
{
	if (path.empty())
		return path;

	std::ostringstream oss;
	std::istringstream iss(path);
	std::string segment;

	bool firstSegment = true;

	while (std::getline(iss, segment, '/')) {
		if (!firstSegment)
			oss << '/';

		if (!segment.empty())
			oss << percentEncode(segment);

		firstSegment = false;
	}

	if (path.back() == '/')
		oss << '/';

	return oss.str();
}
