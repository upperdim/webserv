/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IHandler.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 14:36:57 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/08 13:21:56 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IHANDLER_HPP
#define IHANDLER_HPP

#include "Request.hpp"
#include "Response.hpp"

class IHandler
{
public:
	virtual	~IHandler() {}

	virtual Response	handle(const Request& request) =0;
};

#endif
