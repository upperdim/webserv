/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 14:26:27 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/14 16:37:52 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <map>
#include <functional>
#include "Server.hpp"
#include "AHandler.hpp"
#include "ErrorHandler.hpp"
#include "GetHandler.hpp"

class Server;
class AHandler;

class Router
{
public:
	Router(const Server& _server);
	~Router();

	AHandler*	route(const Request& request);

private:
	static const std::map<std::string, std::function<AHandler*(const Server&)>>	m_handlers;
	const Server&																m_server;

	Router(const Router& other);
	Router&	operator=(const Router& rhs);

	static AHandler*	createErrorHandler(const Server& _server);
	static AHandler*	createGetHandler(const Server& _server);
};

#endif
