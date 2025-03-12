/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorHandler.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 18:19:21 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/12 15:16:17 by nmihaile         ###   ########.fr       */
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
	Response	response;
	int			status_code = request.getStatusCode();

	if (status_code < WSSC_BAD_REQUEST)
		status_code = WSSC_INTERNAL_SERVER_ERROR;

	createErrorResponse(response, status_code);
	return (response);
}
