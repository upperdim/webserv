/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Validator.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 11:09:33 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/11 11:21:15 by nmihaile         ###   ########.fr       */
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
