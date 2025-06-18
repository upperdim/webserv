/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Validate.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 20:00:15 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/18 16:30:31 by nmihaile         ###   ########.fr       */
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
	setStatusCode(_status_code, WSSC_INTERNAL_SERVER_ERROR);
	return (false);
}

bool	Validate::method(std::string& str, HTTP::Method& method, int& _status_code)
{
	if (str == "GET\0") {
		method = HTTP::Method::GET;
		return true;
	} else if (str == "POST\0") {
		method = HTTP::Method::POST;
		return true;
	} else if  (str == "DELETE\0") {
		method = HTTP::Method::DELETE;
		return true;
	}

	std::string ustr = str;
	std::transform(ustr.begin(), ustr.end(), ustr.begin(), [](char c){
		return (std::toupper(c));
	});

	if (ustr == "GET\0" || ustr == "POST\0" || ustr == "DELETE\0") {
		method = HTTP::strToMethod(str);
		setStatusCode(_status_code, WSSC_BAD_REQUEST);
		return false;
	}
		
	method = HTTP::Method::GET;
	setStatusCode(_status_code, WSSC_METHOD_NOT_ALLOWED);
	return false;
}


/* ************************************************************************** */
/* ************************************************************************** */


void	Validate::setStatusCode(int& _status_code, int _new_status)
{
	if (_status_code == 0)
		_status_code = _new_status;
}
