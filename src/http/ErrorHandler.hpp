/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorHandler.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 18:19:23 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/18 14:38:56 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORHANDLER_HPP
#define ERRORHANDLER_HPP

#include "Config.hpp"
#include "AHandler.hpp"
#include "Response.hpp"

class ErrorHandler : public AHandler
{
public:
	ErrorHandler(const ServerBlock& _serverBlock);
	virtual ~ErrorHandler();

	virtual Response	handle(const Request& request);

private:
};

#endif
