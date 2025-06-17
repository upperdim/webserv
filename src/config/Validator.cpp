/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Validator.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tunsal <tunsal@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 11:09:33 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/17 17:19:48 by tunsal           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Validator.hpp"

Validator::~Validator()
{
}


/* ************************************************************************** */
/* ************************************************************************** */


bool Validator::isIPAddr(const std::string& str)
{
	if (str.empty())
		return false;
	if (str == "localhost")
		return true;

	size_t	pos = 0;
	int		octetCount = 0;
	int		currentOctet;

	if (pos + 7 > str.length())
		return false;

	while (pos < str.length() && octetCount < 4) {
		currentOctet = 0;
		int digits = 0;
		
		while (pos < str.length() && std::isdigit(str[pos]) && digits < 3) {
			currentOctet = currentOctet * 10 + (str[pos++] - '0');
			++digits;
		}

		if (currentOctet > 255)
			return false;
		
		if (octetCount < 3) {
			if (pos >= str.length() || str[pos] != '.')
				return false;
			++pos;
		}

		++octetCount;
	}

	return octetCount == 4 && (pos == str.length() || !std::isdigit(str[pos]));
}

bool	Validator::isDomainName(const std::string& str)
{
	if (str.empty() || str.length() > 253)
		return false;
	
	size_t	start	= 0;
	size_t	end		= str.find('.');
	bool	isValid	= true;

	while (isValid && end != std::string::npos) {
		// check label
		std::string label = str.substr(start, end - start);

		// check label length
		if (label.empty() || label.length() > 63) {
			isValid = false;
			break ;
		}
		
		// check first and last characters are alphanumeric
		if (!std::isalnum(label.front()) || !std::isalnum(label.back())) {
			isValid = false;
			break ;
		}

		// check each character in label
		for (std::string::const_iterator it = label.cbegin(); it != label.end(); ++it) {
			if (!std::isalnum(*it) && *it != '-') {
				isValid = false;
				break ;
			}
		}

		start = end + 1;
		end = str.find('.', start);
	}

	// check last label
	std::string label = str.substr(start);
	if (label.empty() || label.length() > 63) {
		isValid = false;
	} else {
		if (!std::isalnum(label.front()) || !std::isalnum(label.back())) {
			isValid = false;
		} else {
			for (std::string::const_iterator it = label.cbegin(); it != label.end(); ++it) {
				if (!std::isalnum(*it) && *it != '-') {
					isValid = false;
					break ;
				}
			}
		}
	}

	return isValid;
}

bool	Validator::isValidServerName(const std::string& str)
{
	if (isDomainName(str))
		return true;
	return false;
}

bool	Validator::isValidPort(const std::string& str, unsigned int& port)
{
	try {
		port = std::stoi(str);
	} catch(...) {
		return false;
	}
	if (port == 0 || port > 65535)
		return false;
	return true;
}

bool Validator::isValidErrorPageNbr(const std::string& str, int& nbr)
{
	try {
		nbr = std::stoi(str);
	} catch(...) {
		return false;
	}
	if (nbr >= 300 && nbr <= 599)
		return true;
	return false;
}

bool	Validator::isValidMethod(const std::string& str, HTTP::Method& method)
{
	if (str == "GET") {
		method = HTTP::Method::GET;
		return true;
	} else if (str == "POST") {
		method = HTTP::Method::POST;
		return true;
	} else if (str == "DELETE") {
		method = HTTP::Method::DELETE;
		return true;
	}
	return false;
}

bool	Validator::isValidToggle(const std::string& str, bool& toggle)
{
	if (str == "on" || str == "true") {
		toggle = true;
		return true;
	} else if (str == "off" || str == "false") {
		toggle = false;
		return true;
	}
	return false;
}

bool	Validator::isValidExtension(const std::string& ext)
{
	if (ext.empty())
		return false;
	if (ext[0] != '.')
		return false;
	size_t len = ext.length();
	if (len < 2 || len > 256)
		return false;
	for (size_t i = 1; i < len; ++i ) {
		char c = ext[i];
		if (!std::isalnum(c) && c != '-' && c != '_')
			return false;
	}
	return true;
}
