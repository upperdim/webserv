/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 18:06:23 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/25 11:07:12 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <unordered_map>
#include "HTTP.hpp"
#include "Config.hpp"

class Request
{
public:
	Request(const ServerBlock& _serverBlock);
	~Request();

	enum class State {
		READING_REQUEST_LINE,
		READING_HEADERS,
		READING_BODY,
		COMPLETE
	};

	// move assignment operator
	Request&				operator=(Request&& rhs);

	void					append(const char *buf, const size_t bytes_read);
	void					reset(void);

	Request::State			getState(void);
	void					setState(State state);

	void					setError(int _statusCode);
	bool					error(void);

	void					setComplete(void);
	bool					complete(void);

	int						getStatusCode(void) const;
	std::string				getRequestTarget(void) const;
	const LocationBlock&	locationBlock();
	bool					isAllowedMethod(void);

	std::string										rawRequest;
	bool											isComplete;

	HTTP::Method									method;
	int												statusCode;
	std::string										requestTarget;
	std::string										URI;		//	decoded and sanatized requesttarget
	std::string										protokoll;
	std::unordered_map<std::string, std::string>	headers;

	const ServerBlock&								serverBlock;

private:
	State											m_state;
	bool											m_error;
	LocationBlock*									m_locationBlock;
};

#endif
