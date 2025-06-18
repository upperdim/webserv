/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Validate.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 20:00:17 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/18 16:25:31 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VALIDATE_HPP
#define VALIDATE_HPP

#include <string>
#include <algorithm>
#include "Http.hpp"

class Validate
{
public:
	~Validate();

	static bool	sstream(bool failed, int& status_code);
	static bool	method(std::string& str, HTTP::Method& method, int& status_code);
	
private:
	Validate();
	Validate(const Validate& other);
	Validate&	operator=(const Validate& rhs);

	static void	setStatusCode(int& _status_code, int _new_status);
};

#endif
