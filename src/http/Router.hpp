/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 14:26:27 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/18 14:48:46 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <map>
#include <functional>
#include "Http.hpp"
#include "AHandler.hpp"
#include "ErrorHandler.hpp"
#include "GetHandler.hpp"

class AHandler;

class Router
{
public:
	Router(const ServerBlock& _serverBlock);
	~Router();

	AHandler*	route(const Request& request);

private:
	static const std::map<HTTP::Method, std::function<AHandler*(const ServerBlock&)>>	m_handlers;
	const ServerBlock&																	m_serverBlock;

	Router(const Router& other);
	Router&	operator=(const Router& rhs);

	static AHandler*	createErrorHandler(const ServerBlock& _serverBlock);
	static AHandler*	createGetHandler(const ServerBlock& _serverBlock);
};

#endif
