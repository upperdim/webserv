/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tunsal <tunsal@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 18:06:23 by nmihaile          #+#    #+#             */
/*   Updated: 2025/07/01 19:07:02 by tunsal           ###   ########.fr       */
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

	enum class ParsingState {
		READING_REQUEST_LINE,
		READING_HEADERS,
		READING_BODY,
		COMPLETE
	};

	void					setError(int _statusCode);
	bool					error(void);

	void					setComplete(void);
	bool					complete(void);

	int						getStatusCode(void) const;
	std::string				getRequestTarget(void) const;
	const LocationBlock&	locationBlock();
	bool					isAllowedMethod(void);

	std::string										rawRequest;
	ParsingState									parsingState;
	
	HTTP::Method									method;
	int												statusCode;
	std::string										requestTarget;
	std::string										URI;		//	decoded and sanatized requesttarget
	std::string										protokoll;
	std::unordered_map<std::string, std::string>	headers;

	const ServerBlock&								serverBlock;

private:
	bool											m_error;
	LocationBlock*									m_locationBlock;
};

#endif
