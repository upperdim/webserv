/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 14:26:27 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/09 18:23:32 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <map>
#include <functional>
#include "AHandler.hpp"
#include "ErrorHandler.hpp"
#include "GetHandler.hpp"

class Router
{
public:
	Router();
	~Router();

	AHandler*	route(const Request& request);

private:
	static const std::map<std::string, std::function<AHandler*(void)>>	m_handlers;

	Router(const Router& other);
	Router&	operator=(const Router& rhs);

	static AHandler*	createErrorHandler(void);
	static AHandler*	createGetHandler(void);
};

#endif
