/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   trimWhitespaces.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 12:07:20 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/02 12:18:35 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

void	trimWhitespaces(std::string& str)
{
	size_t	start	= 0;
	size_t	end		= str.length();
	start = str.find_first_not_of("\t\n\v\f\r ");
	if (start == std::string::npos)
	{
		str.clear();
		return ;
	}
	end = str.find_last_not_of("\t\n\v\f\r ");
	str = str.substr(start, end - start + 1);
}
