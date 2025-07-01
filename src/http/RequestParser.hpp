/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 14:13:20 by nmihaile          #+#    #+#             */
/*   Updated: 2025/07/01 20:23:31 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include <string>
#include "colors.hpp"
#include "Request.hpp"

class RequestParser
{
public:
	static void					parseNext(Request& request);

private:
	static void					parseRequestLine(Request& request);
	static void					parseHeader(Request& request);
	static void					parseBody(Request& request);

	static bool					validateHttpMethod(std::string& methodStr, Request& request);
	static bool					validateRequestTarget(Request& request);
	static bool					validateProtokoll(Request& request);

	static bool					validRawCharacters(const std::string& requestTarget);
	static bool					validDecodedCharacters(const std::string& uri);
	static const std::string	truncateQueryAndFragments(const std::string& requestTarget);
	static bool					percentDecoding(const std::string& requestTarget, std::string& destURI);
	static int					hexToInt(const char c);

	static bool					isRelativeForm_EnsureLeadingSlash(std::string& uri);
	static bool					removeDotSegments(std::string& uri);
	static void					popLastSegment(std::string& oBuf);
	static bool					collapseDuplicateSlashes(std::string& oBuf);

	static bool					splitHeaderField(std::string& line, std::pair<std::string, std::string>& headerField);
	static bool					isValidFieldNameChar(const char c);
	static bool					isValidFieldValueChar(const char c);
	static bool					validateRequiredHeaderFields(Request& request);

};

#endif
