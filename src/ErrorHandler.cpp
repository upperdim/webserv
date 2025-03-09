/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorHandler.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 18:19:21 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/09 18:30:34 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ErrorHandler.hpp"

ErrorHandler::ErrorHandler()
{
}

ErrorHandler::~ErrorHandler()
{
}


/* ************************************************************************** */
/* ************************************************************************** */


Response	ErrorHandler::handle(const Request& request)
{
	Response response;

	createErrorResponse(response, request.status_code);
	return (response);
}
