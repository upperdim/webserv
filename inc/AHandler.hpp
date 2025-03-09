/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AHandler.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 14:36:57 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/09 18:08:44 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AHANDLER_HPP
#define AHANDLER_HPP

#include "StatusCodes.hpp"
#include "Request.hpp"
#include "Response.hpp"

class AHandler
{
public:
	virtual	~AHandler();

	virtual Response	handle(const Request& request) =0;
	virtual	bool		createErrorResponse(Response& response, int _status_code);
};

#endif
