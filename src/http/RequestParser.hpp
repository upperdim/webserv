/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 14:13:20 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/22 19:42:33 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include <string>
#include <istream>
#include "HTTP.hpp"
#include "colors.hpp"
#include "Request.hpp"
#include "Log.hpp"

class RequestParser
{
public:
	RequestParser();
	~RequestParser();

	void	append(const char *buf, const size_t bytes_read);

private:
	void	parseNext(void);
	void	parseRequestLine(void);
	void	parseHeader(void);

	bool	validateHttpMethod(std::string& methodStr, HTTP::Method& dest, int& status_code);

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

	Request		m_request;
	
};

#endif