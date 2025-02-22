/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventLoop.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 11:39:38 by nmihaile          #+#    #+#             */
/*   Updated: 2025/02/22 11:42:03 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EVENTLOOP_HPP
#define EVENTLOOP_HPP

class EventLoop
{
public:
	EventLoop();
	~EventLoop();

	void	run(void);
	void	registerFd(void);
	void	unregisterFd(void);

private:
	EventLoop(const EventLoop& other);
	EventLoop&	operator=(const EventLoop& rhs);
};

#endif