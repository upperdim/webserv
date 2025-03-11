/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 13:04:46 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/11 12:00:24 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Http.hpp"

const std::map<int, std::string>	HTTP::status_messages = {
	{100,	"Continue"},
	{101,	"Wwitching Protocols"},
	{102,	"Processing"},
	{103,	"Early Hints"},

	{200,	"OK"},
	{201,	"Created"},
	{202,	"Accepted"},
	{203,	"Non-Authoritative Information"},
	{204,	"No Content"},
	{205,	"Reset Content"},
	{206,	"Partial Content"},
	{207,	"Multi-Status"},
	{208,	"Already Reported"},
	{226,	"IM Used"},

	{300,	"Multiple Choices"},
	{301,	"Moved Permanently"},
	{302,	"Found"},
	{303,	"See Other"},
	{304,	"Not Modified"},
	{307,	"Temporary Redirect"},
	{308,	"Permanent Redirect"},

	{400,	"Bad Request"},
	{401,	"Unauthorized"},
	{402,	"Payment Required"},
	{403,	"Forbidden"},
	{404,	"Not Found"},
	{405,	"Method Not Allowed"},
	{406,	"Not Acceptable"},
	{407,	"Proxy Authentication Required"},
	{408,	"Request Timeout"},
	{409,	"Conflict"},
	{410,	"Gone"},
	{411,	"Length Required"},
	{412,	"Precondition Failed"},
	{413,	"Content Too Large"},
	{414,	"URI Too Long"},
	{415,	"Unsupported Media Type"},
	{416,	"Range Not Satisfiable"},
	{417,	"Expectation Failed"},
	{418,	"I'm a teapot"},
	{421,	"Misdirected Request"},
	{422,	"Unprocessable Content"},
	{423,	"Locked"},
	{424,	"Failed Dependency"},
	{425,	"Too Early"},
	{426,	"Upgrade Required"},
	{428,	"Precondition Required"},
	{429,	"Too Many Requests"},
	{431,	"Request Header Fields Too Large"},
	{451,	"Unavailable For Legal Reasons"},

	{500,	"internal_server_error"},
	{501,	"not_implemented"},
	{502,	"bad_gateway"},
	{503,	"service_unavailable"},
	{504,	"gateway_timeout"},
	{505,	"http_version_not_supported"},
	{506,	"variant_also_negotiates"},
	{507,	"insufficient_storage"},
	{508,	"loop_detected"},
	{510,	"not_extended"},
	{511,	"network_authentication_required"}
};

const std::map<std::string, std::string>	HTTP::mime_types = {
	{".txt", "text/plain"},
	{".html", "text/html"},
	{".html", "text/html"},
	{".jpg", "image/jpeg"},
	{".jpeg", "image/jpeg"},
	{".png", "image/png"},
	{".json", "application/json"}
};

HTTP::~HTTP()
{
}


/* ************************************************************************** */
/* ************************************************************************** */


std::string HTTP::getStatusMessage(int _status_code)
{
	auto it = status_messages.find(_status_code);
	return ( it != status_messages.end() ? it->second : std::string("Unknown"));
}

std::string	HTTP::getMimeType(const std::string& path)
{
	size_t	pos = path.find_last_of(".");
	if (pos == std::string::npos)
		return ("application/octet-stream");
		
	std::string ext = path.substr(pos);
	auto it = mime_types.find(ext);
	return ( it != mime_types.end() ? it->second : std::string("application/octet-stream") );
}
