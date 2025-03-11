/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 11:29:40 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/11 12:01:43 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <regex>
#include "Request.hpp"
#include "Http.hpp"

class Response
{
public:
	std::string	data;

	Response();
	Response(const Response& other);
	~Response();

	Response&	operator=(const Response& rhs);
private:
};

#endif
