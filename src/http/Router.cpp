/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 14:26:26 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/26 17:43:18 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"

Router::Router(const Server& _server)
	:	m_server(_server)
{
}

Router::~Router()
{
}


/* ************************************************************************** */
/* ************************************************************************** */


const std::map<std::string, std::function<AHandler*(const Server&)>>	Router::m_handlers = {
	{"GET", createGetHandler}
};

AHandler*	Router::route(const Request& request)
{
	if (request.error())
		return ( createErrorHandler(m_server) );

	auto it = m_handlers.find(request.getMethod());
	if (it != m_handlers.end())
		return ( it->second(m_server) );
	return ( createErrorHandler(m_server) );
}


/* ************************************************************************** */
/* ************************************************************************** */


AHandler*	Router::createErrorHandler(const Server& _server)
{
	return ( new ErrorHandler(_server) );
}

AHandler*	Router::createGetHandler(const Server& _server)
{
	return ( new GetHandler(_server) );
}
