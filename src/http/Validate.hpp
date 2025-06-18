/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Validate.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tunsal <tunsal@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 20:00:17 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/18 20:47:19 by tunsal           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VALIDATE_HPP
#define VALIDATE_HPP

#include <string>
#include <algorithm>
#include "HTTP.hpp"
#include "Validator.hpp"

class Validate
{
public:
	~Validate();

	static bool	sstream(bool failed, int& status_code);
	static bool	validateHttpMethod(std::string& methodStr, HTTP::Method& dest, int& status_code);
	
private:
	Validate();
	Validate(const Validate& other);
	Validate&	operator=(const Validate& rhs);

	static void	setStatusCode(int& _status_code, int _new_status);
};

#endif
