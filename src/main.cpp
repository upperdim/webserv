/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 11:44:18 by nmihaile          #+#    #+#             */
/*   Updated: 2025/02/20 18:15:45 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "../inc/Server.hpp"

int	main(void)
{

	Server server;

	std::cout << "\033[92mservering the world wide web…\033[0m" << std::endl;
	while (42)
	{
		server.srv_listen();
	}

	return (0);
}
