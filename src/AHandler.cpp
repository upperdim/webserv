/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AHandler.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 17:47:24 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/12 11:46:49 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AHandler.hpp"

AHandler::~AHandler()
{
}


/* ************************************************************************** */
/* ************************************************************************** */


void	AHandler::createErrorResponse(Response& response, int _status_code)
{
	response.setProtokoll("HTTP/1.1");
	response.setStatus(_status_code, HTTP::getStatusMessage(_status_code));
	response.setBody(HTTP::getStatusMessage(_status_code)); // TODO load an ERROR_PAGE
}
