#include <fstream>
#include <sstream>
#include "webserv.hpp"

std::string readFile(const std::string& filename)
{
	std::ifstream file(filename);
	if (!file)
		throw ( std::runtime_error("Failed to open " + filename) );
	std::stringstream buffer;
	buffer << file.rdbuf();
	return (buffer.str());
}
