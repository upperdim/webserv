/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 14:13:20 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/23 11:46:13 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include <string>
#include <sstream>
#include "HTTP.hpp"
#include "colors.hpp"
#include "Request2.hpp"
#include "Validator.hpp"
#include "Log.hpp"
#include "Utils.hpp"

class RequestParser
{
public:
	RequestParser();
	~RequestParser();

	Request	request;

	void	append(const char *buf, const size_t bytes_read);
	void	reset(void);

	// setters
	void	setError(int statusCode);

	// getters
	bool	error(void);
	bool	complete(void);

private:
	void	parseNext(void);
	void	parseRequestLine(void);
	void	parseHeader(void);

	bool	validateHttpMethod(std::string& methodStr);
	bool	validateRequestTarget(void);
	bool	validateProtokoll(void);

	bool	splitLine(std::string& line, char del, std::pair<std::string, std::string>& headerField);

	enum class State {
		READING_REQUEST_LINE,
		READING_HEADERS,
		READING_BODY,
		COMPLETE
	};

	bool		m_error;
	State		m_state;
	std::string	m_rawRequest;
	
};

#endif