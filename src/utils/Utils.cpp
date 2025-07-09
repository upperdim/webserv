#include "Utils.hpp"
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include "Log.hpp"
#include "unistd.h"

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

bool	Utils::startsWithHttp(const std::string& str)
{
	auto equalCheck = [](char a, char b){
		return std::tolower(static_cast<unsigned char>(a)) ==
		       std::tolower(static_cast<unsigned char>(b));
	};

	static const char* http  = "http";

	if (str.size() >= 4 && std::equal(http, http + 4, str.begin(), equalCheck))
		return true;

	return false;
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

void	Utils::unquote(std::string& str, char quote)
{
	while (str.size() > 2 && str.front() == quote && str.back() == quote) {
		str.erase(0, 1);
		str.erase(str.length() - 1, 1);
	}
}

//	RFC:	5.2.4.  Remove Dot Segments
bool	Utils::removeDotSegments(std::string& uri)
{
	// ensure we have an absolute path startiung with '/'
	if (uri.empty() || uri[0] != '/')
		return false;

	std::string iBuf = uri;
	std::string oBuf;
	oBuf.reserve(uri.size());

	while (!iBuf.empty()) {
		if (Utils::startsWith(iBuf, "../")) {
			iBuf.erase(0, 3);
		} else if (Utils::startsWith(iBuf, "./")) {
			iBuf.erase(0, 2);
		} else if (Utils::startsWith(iBuf, "/./")) {
			iBuf.replace(0, 3, "/");
		} else if (Utils::startsWith(iBuf, "/.") &&
		           (iBuf.size() == 2 || iBuf[2] == '/')) {
			iBuf.replace(0, 2, "/");
		} else if (Utils::startsWith(iBuf,"/../")) {
			iBuf.replace(0, 4, "/");
			popLastSegment(oBuf);
		} else if (Utils::startsWith(iBuf,"/..") &&
		           (iBuf.size() == 3 || iBuf[3] == '/')) {
			iBuf.replace(0, 3, "/");
			popLastSegment(oBuf);
		} else if (iBuf == "." || iBuf == "..") {
			iBuf.clear();
		} else {
			// move first seg from iBuf to oBuf
			size_t pos;
			if (iBuf[0] == '/') {
				if ((pos = iBuf.find("/", 1)) != std::string::npos) {
					oBuf += iBuf.substr(0, pos);
					iBuf.erase(0, pos);
				} else {
					oBuf += iBuf;
					iBuf.clear();
				}
			} else {
				// Unexpected uri segment without leading '/' => _400_BAD_REQUEST
				return false;
			}
		}
	}
	if (oBuf.empty())
		oBuf = "/";
	uri = oBuf;
	return true;
}

bool	Utils::collapseDuplicateSlashes(std::string& oBuf)
{
	size_t write = 0;
	bool prevWasSlash = false;

	for (size_t read = 0; read < oBuf.size(); ++read) {
		if (oBuf[read] == '/') {
			if (!prevWasSlash) {
				oBuf[write++] = '/';
				prevWasSlash = true;
			}
		} else {
			oBuf[write++] = oBuf[read];
			prevWasSlash = false;
		}
	}
	oBuf.resize(write);
	return true;
}


//=============================================================================
// Private Methods
//=============================================================================

void	Utils::popLastSegment(std::string& oBuf)
{
	size_t pos = oBuf.rfind("/");
	if (pos != std::string::npos) {
		if (pos == 0)
			oBuf = "/";
		else
			oBuf.erase(pos);
	}
}
