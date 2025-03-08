/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 14:26:27 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/08 12:32:49 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <map>
#include <functional>
#include "IHandler.hpp"
#include "GetHandler.hpp"

class Router
{
public:
	Router();
	~Router();

	IHandler*	route(const Request& request);

private:
	static const std::map<std::string, std::function<IHandler*(void)>>	m_handlers;

	Router(const Router& other);
	Router&	operator=(const Router& rhs);

	static IHandler*	createGetHandler(void);
};

#endif
