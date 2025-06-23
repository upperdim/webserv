/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request2.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 18:06:23 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/22 19:54:55 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>

class Request
{
public:
	Request();
	~Request();

private:
	int			m_statusCode;
	std::string	m_requestTarget;
	std::string	m_protokoll;

	friend class RequestParser;
};

#endif