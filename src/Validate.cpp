/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Validate.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 20:00:15 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/02 21:02:00 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Validate.hpp"

Validate::~Validate()
{
}


/* ************************************************************************** */
/* ************************************************************************** */


bool	Validate::sstream(bool failed, int& _error_code)
{
	if (failed == false)
		return (true);
	_error_code = WBSC_INTERNAL_SERVER_ERROR;
	return (false);
}

bool	Validate::method(std::string& str, int& _status_code)
{
	if (str == "GET\0" || str == "POST\0" || str == "DELETE\0")
		return (false);

	std::string ustr = str;
	std::transform(ustr.begin(), ustr.end(), ustr.begin(), [](char c){
		return (std::toupper(c));
	});

	if (ustr == "GET\0" || ustr == "POST\0" || ustr == "DELETE\0") {
		_status_code = WBSC_BAD_REQUEST;
		return (true);
	}
		
	_status_code = WBSC_METHOD_NOT_ALLOWED;
	return (true);
}
