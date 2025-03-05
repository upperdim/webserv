/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IHandler.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 14:36:57 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/05 14:39:07 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IHANDLER_HPP
#define IHANDLER_HPP

#include "Request.hpp"

class IHandler
{
public:
	IHandler();
	virtual	~IHandler();

	virtual void	handle(const Request& request) =0;

private:
	IHandler(const IHandler& other);
	IHandler&	operator=(const IHandler& rhs);
};

#endif
