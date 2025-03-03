/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Validate.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 20:00:15 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/03 10:58:47 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Validate.hpp"

Validate::~Validate()
{
}


/* ************************************************************************** */
/* ************************************************************************** */


bool	Validate::sstream(bool failed, int& _status_code)
{
	if (failed == false)
		return (true);
	setStatusCode(_status_code, WBSC_INTERNAL_SERVER_ERROR);
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
		setStatusCode(_status_code, WBSC_BAD_REQUEST);
		return (true);
	}
		
	setStatusCode(_status_code, WBSC_METHOD_NOT_ALLOWED);
	return (true);
}


/* ************************************************************************** */
/* ************************************************************************** */

void	Validate::setStatusCode(int& _status_code, int _new_status)
{
	if (_status_code == 0)
		_status_code = _new_status;
}
