/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/25 17:46:51 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/14 11:40:40 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <sstream>
#include <utility>
#include <stdexcept>
#include "webserv.hpp"
#include "Validate.hpp"

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

	Request();
	~Request();

	void		append(char buf[REQUEST_BUFFER_SIZE], size_t bytes_read);
	bool		complete(void) const;
	int			error(void) const;

	std::string	getRequest(void) const;			// TODO: delete because just used for debugging
	std::string	getRequestLine(void) const;		// TODO: delete because just used for debugging

	int			getStatusCode(void) const;
	std::string	getMethod(void) const;
	std::string	getRequestTarget(void) const;
	void		setComplete();
	void		setError(int _status_code);

private:
	State		m_state;
	int			m_status_code;

	std::string	m_raw_request;
	std::string	m_method;
	std::string	m_request_target;
	std::string	m_HTTP_version;
	HeaderMap	m_headers;
	std::string	m_body;

	Request(const Request& other);
	Request&	operator=(const Request& rhs);

	void	parseNext(void);
	void	parseRequestLine(void);
	void	parseHeader(void);

	bool	splitLine(std::string& line, char del, std::pair<std::string, std::string>& headerField);
};

#endif
