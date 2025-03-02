/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Validate.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 20:00:17 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/02 21:00:36 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VALIDATE_HPP
#define VALIDATE_HPP

#include <string>
#include <algorithm>
#include "StatusCodes.hpp"

class Validate
{
public:
	~Validate();

	static bool	sstream(bool failed, int& _error_code);
	static bool	method(std::string& str, int& _error_code);
	
private:
	Validate();
	Validate(const Validate& other);
	Validate&	operator=(const Validate& rhs);
};

#endif
