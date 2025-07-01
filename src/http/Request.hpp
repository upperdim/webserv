/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 18:06:23 by nmihaile          #+#    #+#             */
/*   Updated: 2025/07/01 20:16:01 by nmihaile         ###   ########.fr       */
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

	enum class ParsingState {
		REQUEST_LINE,
		HEADERS,
		BODY,
		COMPLETE,
		INVALID
	};

	void					setError(int _statusCode);
	bool					error(void);

	int						getStatusCode(void) const;
	std::string				getRequestTarget(void) const;
	const LocationBlock&	locationBlock();
	bool					isAllowedMethod(void);

	std::string										rawRequest;
	ParsingState									parsingState;
	bool											doneReceiving;
	
	HTTP::Method									method;
	int												statusCode;
	std::string										requestTarget;
	std::string										URI;		//	decoded and sanatized requesttarget
	std::string										protokoll;
	std::unordered_map<std::string, std::string>	headers;

	const ServerBlock&								serverBlock;

	// resolved attributes
	size_t											resolvedContentlength;

private:
	bool											m_error;
	LocationBlock*									m_locationBlock;
};

#endif
