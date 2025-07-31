
#ifndef VALIDATOR_HPP
#define VALIDATOR_HPP

#include <string>
#include "HTTP.hpp"

class Validator
{
public:
	~Validator();

	static bool	isIPAddr(const std::string& str);
	static bool	isDomainName(const std::string& str);
	static bool	isValidServerName(const std::string& str);
	static bool	isValidPort(const std::string& str, unsigned int& port);
	static bool isValidErrorPageNbr(const std::string& str, int& nbr);
	static bool	isValidMethod(const std::string& str, HTTP::Method& method);
	static bool	isValidToggle(const std::string& str, bool& toggle);
	static bool	isValidExtension(const std::string& ext);
	static bool	isValidExecutable(const std::string& executable);
	static bool	isValidContentTypeBoundary(std::string& boundary);

private:
	Validator();
};

#endif