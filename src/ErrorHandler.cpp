/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorHandler.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 18:19:21 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/14 18:36:36 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ErrorHandler.hpp"

ErrorHandler::ErrorHandler(const Server& _server) : AHandler(_server)
	// :	m_server(_server)
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
