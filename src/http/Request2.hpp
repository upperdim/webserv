/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request2.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 18:06:23 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/24 17:45:47 by nmihaile         ###   ########.fr       */
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

	enum class State {
		READING_REQUEST_LINE,
		READING_HEADERS,
		READING_BODY,
		COMPLETE
	};

	// move assignment operator
	Request&		operator=(Request&& rhs);

	void			append(const char *buf, const size_t bytes_read);
	void			reset(void);

	Request::State	getState(void);
	void			setState(State state);

	void			setError(int statusCode);
	bool			error(void);

	void			setComplete(void);
	bool			complete(void);

	int						getStatusCode(void) const;
	std::string				getRequestTarget(void) const;
	const LocationBlock&	getLocation(const ServerBlock& serverBlock) const;

	std::string				rawRequest;
	HTTP::Method			method;

private:
	State											m_state;
	bool											m_error;

	int												m_statusCode;

	std::string										m_requestTarget;
	std::string										m_protokoll;
	std::unordered_map<std::string, std::string>	m_headers;

	std::string										m_URI;		//	decoded and sanatized requesttarget

	friend class RequestParser;
};

#endif
