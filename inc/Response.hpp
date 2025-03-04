/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 11:29:40 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/04 12:32:08 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include "Request.hpp"
#include "StatusCodes.hpp"

class Response
{
public:
	Response();
	~Response();

	std::string	data;

	void	create(const Request& request);

private:
	Response(const Response& other);
	Response&	operator=(const Response& rhs);

	std::string	statusLine(const Request& request);
};

#endif
