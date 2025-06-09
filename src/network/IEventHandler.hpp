/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IEventHandler.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 19:42:07 by nmihaile          #+#    #+#             */
/*   Updated: 2025/02/23 12:08:34 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IEVENTHANDLER_HPP
#define IEVENTHANDLER_HPP

#include "EventManager.hpp"

class EventManager;

class IEventHandler
{
public:
	int	socket_fd;
	
	virtual ~IEventHandler() {};

	virtual void handleReadEvent(EventManager& event_manager)		=0;
	virtual void handleWriteEvent(EventManager& event_manager)		=0;
	virtual void handleErrorEvent(EventManager& event_manager)		=0;
	virtual void handleDisConnectEvent(EventManager& event_manager)	=0;
};

#endif
