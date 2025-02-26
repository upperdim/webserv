/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/25 17:46:51 by nmihaile          #+#    #+#             */
/*   Updated: 2025/02/26 16:36:33 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <sstream>
#include "Log.hpp"

class Request
{
public:
	enum class State
	{
		READING_REQUEST_LINE,
		READING_HEADERS,
		READING_BODY,
		COMPLETE,
		ERROR
	};

	State	state;

	Request();
	~Request();

	void	append(char buf[REQUEST_BUFFER_SIZE], size_t bytes_read);
	
private:
	std::string	m_raw_request;

	std::string	m_method;
	std::string	m_request_target;
	std::string	m_HTTP_version;

	Request(const Request& other);
	Request&	operator=(const Request& rhs);

	void	parseNext(void);
	void	parseRequestLine(void);
};

#endif
