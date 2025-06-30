/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tunsal <tunsal@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 13:25:25 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/30 00:32:48 by tunsal           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "ServerEngine.hpp"

void	handleAbort(int signum)
{
	(void)signum;

	ServerEngine::isRunning = false;

	std::cout << "\033[2K\033[1G";
	// std::cout << "\033[K";
	std::cout.flush();
}
