/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 11:29:40 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/12 14:43:12 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include "webserv.hpp"
#include "Request.hpp"
#include "Http.hpp"

class Response
{
public:

	Response();
	Response(const Response& other);
	~Response();

	Response&	operator=(const Response& rhs);
	void		setProtokoll(const std::string& _protokoll);
	void		setStatus(const int& _status_code, const std::string& _status_msg);
	void		addHeader(const std::string& key, const std::string& value);
	void		setBody(const std::string& _body);
	std::string	to_string(void) const;

private:
	std::string	m_protokoll;
	int			m_status_code;
	std::string	m_status_msg;
	HeaderMap	m_headers;
	std::string	m_body;
};

#endif
