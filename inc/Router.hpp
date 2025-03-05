/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 14:26:27 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/05 15:02:15 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <unordered_map>
#include "IHandler.Hpp"
#include "GetHandler.hpp"

class Router
{
public:
	Router();
	~Router();

	IHandler*	route(const Request request);

private:
	std::unordered_map<std::string, IHandler>	m_handlers;

	Router(const Router& other);
	Router&	operator=(const Router& rhs);
};

#endif
