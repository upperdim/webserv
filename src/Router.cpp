/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 14:26:26 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/12 15:13:16 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"

Router::Router()
{
}

Router::~Router()
{
}


/* ************************************************************************** */
/* ************************************************************************** */


const std::map<std::string, std::function<AHandler*(void)>>	Router::m_handlers = {
	{"GET", createGetHandler},
	{"ERROR", createErrorHandler}
};

AHandler*	Router::route(const Request& request)
{
	if (request.error())
		return ( createErrorHandler() );

	auto it = m_handlers.find(request.getMethod());
	if (it != m_handlers.end())
		return ( it->second() );
	return ( createErrorHandler() );
}


/* ************************************************************************** */
/* ************************************************************************** */


AHandler*	Router::createErrorHandler(void)
{
	return ( new ErrorHandler() );
}

AHandler*	Router::createGetHandler(void)
{
	return ( new GetHandler() );
}
