/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 13:25:25 by nmihaile          #+#    #+#             */
/*   Updated: 2025/02/26 13:32:26 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.h"

void	handleAbort(int signum)
{
	g_running = false;
	(void)signum;

	std::cout << "\033[2K\033[1G";
	// std::cout << "\033[K";
	std::cout.flush();
}
