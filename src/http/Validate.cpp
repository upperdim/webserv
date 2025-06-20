/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Validate.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 20:00:15 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/19 16:27:28 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Validate.hpp"

Validate::~Validate()
{
}


/* ************************************************************************** */
/* ************************************************************************** */


bool	Validate::validateHttpMethod(std::string& methodStr, HTTP::Method& dest, int& _status_code)
{
	if (Validator::isValidMethod(methodStr, dest))
		return true;

	std::string ustr = methodStr;
	std::transform(ustr.begin(), ustr.end(), ustr.begin(), [](char c){
		return (std::toupper(c));
	});

	if (ustr == "GET\0" || ustr == "POST\0" || ustr == "DELETE\0") {
		dest = HTTP::strToMethod(methodStr);
		setStatusCode(_status_code, WSSC_BAD_REQUEST);
		return false;
	}
		
	dest = HTTP::Method::GET;
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
