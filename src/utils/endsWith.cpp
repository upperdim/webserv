/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   endsWith.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 13:21:19 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/10 13:24:50 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

bool	endsWith(const std::string& str, const std::string& suffix)
{
	if (str.size() < suffix.size())
		return (false);
	return ( std::equal(suffix.rbegin(), suffix.rend(), str.rbegin()) );
}
