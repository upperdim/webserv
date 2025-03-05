/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/25 17:46:51 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/05 12:09:39 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <stdexcept>
#include "Validate.hpp"
#include "Log.hpp"

class Request
{
public:
	typedef std::unordered_map<std::string, std::string> HeaderMap;
	enum class State
	{
		READING_REQUEST_LINE,
		READING_HEADERS,
		READING_BODY,
		COMPLETE,
		ERROR
	};

	State	state;
	int		status_code;

	Request();
	~Request();

	void		append(char buf[REQUEST_BUFFER_SIZE], size_t bytes_read);
	bool		complete(void) const;
	int			error(void) const;

	std::string	getRequest(void) const;
	std::string	getRequestLine(void) const;
	void		setComplete();
	void		setError(State _state, int _status_code);
	
private:
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
