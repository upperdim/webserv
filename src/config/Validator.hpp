/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Validator.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 11:08:19 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/11 11:20:23 by nmihaile         ###   ########.fr       */
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

private:
	Validator();
};

#endif