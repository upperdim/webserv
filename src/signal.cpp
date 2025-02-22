/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 13:25:25 by nmihaile          #+#    #+#             */
/*   Updated: 2025/02/21 14:54:35 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.h"
#include <iostream>

void	handleAbort(int signum)
{
	g_running = false;
	(void)signum;
	// if (sig == SIGINT || sig == SIGQUIT)
	// std::cout << "QUIT\n";
}
