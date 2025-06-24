/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 14:13:20 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/24 16:48:13 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include <string>
#include "colors.hpp"
#include "Request2.hpp"

class RequestParser
{
public:
	~RequestParser();

	static void	parseNext(Request& request);

private:
	RequestParser();
	
	static void					parseRequestLine(void);
	staticvoid					parseHeader(void);

	static bool					validateHttpMethod(std::string& methodStr);
	static bool					validateRequestTarget(void);
	static bool					validateProtokoll(void);

	static bool					validRawCharacters(const std::string& requestTarget);
	static bool					validDecodedCharacters(const std::string& uri);
	static const std::string	truncateQueryAndFragments(const std::string& requestTarget);
	static bool					percentDecoding(const std::string& requestTarget, std::string& destURI);
	static int					hexToInt(const char c);

	static bool					isRelativeForm_EnsureLeadingSlash(std::string& uri);
	static bool					removeDotSegments(std::string& uri);
	static void					popLastSegment(std::string& oBuf);
	static bool					collapseDuplicateSlashes(std::string& oBuf);

	static bool					splitLine(std::string& line, char del, std::pair<std::string, std::string>& headerField);
	
};

#endif