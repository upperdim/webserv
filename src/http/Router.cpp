/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 14:26:26 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/18 14:44:04 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"

Router::Router(const ServerBlock& _serverBlock)
	:	m_serverBlock(_serverBlock)
{
}

Router::~Router()
{
}


/* ************************************************************************** */
/* ************************************************************************** */


const std::map<HTTP::Method, std::function<AHandler*(const ServerBlock&)>>	Router::m_handlers = {
	{HTTP::Method::GET, createGetHandler}
};

AHandler*	Router::route(const Request& request)
{
	if (request.error())
		return ( createErrorHandler(m_serverBlock) );

	auto it = m_handlers.find(request.getMethod());
	if (it != m_handlers.end())
		return ( it->second(m_serverBlock) );
	return ( createErrorHandler(m_serverBlock) );
}


/* ************************************************************************** */
/* ************************************************************************** */


AHandler*	Router::createErrorHandler(const ServerBlock& _serverBlock)
{
	return ( new ErrorHandler(_serverBlock) );
}

AHandler*	Router::createGetHandler(const ServerBlock& _serverBlock)
{
	return ( new GetHandler(_serverBlock) );
}
