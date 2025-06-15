/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Validator.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 11:08:19 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/15 17:58:21 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VALIDATOR_HPP
#define VALIDATOR_HPP

#include <string>
#include "Http.hpp"

class Validator
{
public:
	~Validator();

	static bool	isIPAddr(const std::string& str);
	static bool	isDomainName(const std::string& str);
	static bool	isValidServerName(const std::string& str);
	static bool	isValidPort(const std::string& str, unsigned int& port);
	static bool isValidErrorPageNbr(const std::string& str, int& nbr);
	static bool	isValidMethod(const std::string& str, HTTP::Method& method);
	static bool	isValidToggle(const std::string& str, bool& toggle);

private:
	Validator();
};

#endif