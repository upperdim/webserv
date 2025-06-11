/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Validator.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 11:09:33 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/11 18:45:04 by nmihaile         ###   ########.fr       */
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
