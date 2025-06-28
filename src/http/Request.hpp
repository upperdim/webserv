/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 18:06:23 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/28 09:46:42 by nmihaile         ###   ########.fr       */
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
	Request&		operator=(Request&& rhs);

	void			append(const char *buf, const size_t bytes_read);
	void			reset(void);

	Request::State	getState(void);
	void			setState(State state);

	void			setError(int _statusCode);
	bool			error(void);

	void			setComplete(void);
	bool			isComplete(void);

	// here I use some OOP trickery and uses locationBlock as a privat member
	// and access it only through the locationBlock() method.
	// Because Location can change from request to request it is a pointer and
	// can be set to NULL (on inital setup and on reset).
	// In the locationBlock() method I check the current state of
	// m_locationBlock and if it is NULL I set a new m_locationBlock or set it
	// to a default one and return this locationBlock.
	// Through this private member and locationBlock getter, which I will not
	// name getLocationBlock() but rather locationBlock() like a variable name
	// I protect m_locationBlock.
	// what do you think, do you like it?
	const LocationBlock&	locationBlock();
	bool					isAllowedMethod(void);

	std::string										rawRequest;

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
	bool											m_error;
	LocationBlock*									m_locationBlock;
};

#endif
