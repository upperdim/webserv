/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request2.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 18:06:23 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/23 12:13:23 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <unordered_map>
#include "HTTP.hpp"
#include "Config.hpp"

// Forward declaration
class RequestParser;

class Request
{
public:
	Request();
	~Request();

	// move assignment operator
	Request&	operator=(Request&& rhs);

	int						getStatusCode(void) const;
	HTTP::Method			getMethod(void) const;
	std::string				getRequestTarget(void) const;
	const LocationBlock&	getLocation(const ServerBlock& serverBlock) const;

private:
	int												m_statusCode;
	std::string										m_requestTarget;
	std::string										m_protokoll;

	HTTP::Method									m_method;
	std::unordered_map<std::string, std::string>	m_headers;

	friend class RequestParser;
};

#endif