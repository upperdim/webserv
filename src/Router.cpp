/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 14:26:26 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/08 13:18:40 by nmihaile         ###   ########.fr       */
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


const std::map<std::string, std::function<IHandler*(void)>>	Router::m_handlers = {
	{"GET", createGetHandler}
};

IHandler*	Router::route(const Request& request)
{
	auto it = m_handlers.find(request.getMethod());
	if (it != m_handlers.end())
		return ( it->second() );
	return ( nullptr );
}


/* ************************************************************************** */
/* ************************************************************************** */


IHandler*	Router::createGetHandler(void)
{
	return ( new GetHandler() );
}
