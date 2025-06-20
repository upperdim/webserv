/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/25 17:46:51 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/19 17:19:55 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <sstream>
#include <utility>
#include <stdexcept>
#include "webserv.hpp"
#include "HTTP.hpp"
#include "Config.hpp"
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

	void					append(char buf[REQUEST_BUFFER_SIZE], size_t bytes_read);
	bool					complete(void) const;
	int						error(void) const;

	std::string				getRequest(void) const;			// TODO: delete because just used for debugging
	std::string				getRequestLine(void) const;		// TODO: delete because just used for debugging

	int						getStatusCode(void) const;
	HTTP::Method			getMethod(void) const;
	std::string				getRequestTarget(void) const;
	const LocationBlock&	getLocation(const ServerBlock& serverBlock) const;
	void					setComplete();
	void					setError(int _status_code);

private:
	State			m_state;
	int				m_statusCode;

	std::string		m_rawRequest;
	HTTP::Method	m_method;
	std::string		m_requestTarget;
	std::string		m_HTTPversion;
	HeaderMap		m_headers;
	std::string		m_body;

	Request(const Request& other);
	Request&	operator=(const Request& rhs);

	void	parseNext(void);
	void	parseRequestLine(void);
	void	parseHeader(void);

	bool	splitLine(std::string& line, char del, std::pair<std::string, std::string>& headerField);
};

#endif
