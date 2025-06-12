/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Validator.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 11:08:19 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/12 10:42:12 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VALIDATOR_HPP
#define VALIDATOR_HPP

#include <string>

class Validator
{
public:
	~Validator();

	static bool	isIPAddr(const std::string& str);
	static bool	isDomainName(const std::string& str);
	static bool	isValidServerName(const std::string& str);
	static bool	isValidPort(const std::string& str, unsigned int& port);

private:
	Validator();
};

#endif