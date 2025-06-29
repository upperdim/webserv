/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 18:06:23 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/29 21:23:19 by nmihaile         ###   ########.fr       */
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
		COMPLETE,
		INVALID
	};

	void					append(const char *buf, const size_t bytes_read);
	void					reset(void);

	Request::State			getState(void);
	void					setState(State state);
	void					setError(int _statusCode);
	void					setComplete(void);
	void					resolveRequestContext(void);

	const LocationBlock&	locationBlock();
	bool					isAllowedMethod(void);

	bool											isComplete;
	std::string										rawRequest;

	// raw request attributes
	HTTP::Method									method;
	int												statusCode;
	std::string										requestTarget;
	std::string										protokoll;
	std::unordered_map<std::string, std::string>	headers;

	const ServerBlock&								serverBlock;

	// matched and resolved attributes
	std::string										URI;		//	decoded and sanatized requesttarget
	std::string										resolvedPath;

private:
	State											m_state;
	LocationBlock*									m_locationBlock;
};

#endif
