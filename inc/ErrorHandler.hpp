/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorHandler.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 18:19:23 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/09 18:20:45 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORHANDLER_HPP
#define ERRORHANDLER_HPP

#include "AHandler.hpp"

class ErrorHandler : public AHandler
{
public:
	ErrorHandler();
	virtual ~ErrorHandler();

	virtual Response	handle(const Request& request);

private:
};

#endif
