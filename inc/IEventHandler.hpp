/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IEventHandler.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 19:42:07 by nmihaile          #+#    #+#             */
/*   Updated: 2025/02/22 19:46:00 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IEVENTHANDLER_HPP
#define IEVENTHANDLER_HPP

class IEventHandler
{
public:
	virtual ~IEventHandler() {};

	virtual void handleReadEvent()	=0;
	virtual void handleWriteEvent()	=0;
	virtual void handleErrorEvent()	=0;
};

#endif
