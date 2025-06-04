/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readFile.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 15:05:58 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/04 15:10:56 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

std::string readFile(const std::string& filename)
{
	std::ifstream file(filename);
	if (!file)
		throw ( std::runtime_error("Failed to open" + filename) );
	std::stringstream buffer;
	buffer << file.rdbuf();
	return (buffer.str());
}
