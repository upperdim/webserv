#include "HTTP.hpp"

const std::map<int, std::string>	HTTP::m_status_messages = {
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

	{500,	"Internal Server Error"},
	{501,	"Not Implemented"},
	{502,	"Bad Gateway"},
	{503,	"Service Unavailable"},
	{504,	"Gateway Timeout"},
	{505,	"HTTP Version Not Supported"},
	{506,	"Variant Also Negotiates"},
	{507,	"Insufficient Storage"},
	{508,	"Loop Detected"},
	{510,	"Not Extended"},
	{511,	"Network Authentication Required"}
};

const std::map<std::string, std::string>	HTTP::m_mime_types = {
	{".txt", "text/plain"},
	{".htm", "text/html"},
	{".html", "text/html"},
	{".css", "text/css"},
	{".jpg", "image/jpeg"},
	{".jpeg", "image/jpeg"},
	{".png", "image/png"},
	{".json", "application/json"}
};

const std::string	HTTP::m_error_page_template = {
"<html>"
"<head><title>{{STATUS_CODE}} - {{REASON_PHRASE}}</title></head>"
"<body style=\"display: flex; flex-direction: column; align-items: center; margin-top: 2em; background-color: oklch(0.145 0 0); font-family:'Gill Sans', 'Gill Sans MT', Calibri, 'Trebuchet MS', sans-serif; line-height: 100%; text-align: center;\">"
"	<h2 style=\"display: flex; flex-direction: row; margin: 0;\">"
"		<div style=\"display: block; margin: 0 0.25em; color: oklch(0.656 0.241 354.308); font-weight: 700;\">{{STATUS_CODE}}</div>"
"		<div style=\"display: block; margin: 0 0.25em; color: oklch(0.823 0.12 346.018); font-weight: 300; \">{{REASON_PHRASE}}</div>"
"	</h2>"
"	<div style=\"display: block; margin: 1em 0; width:50%; height: 1px; border-top-style: solid; border-top-width: 1px; border-color: oklch(0.205 0 0);\"></div>"
"	<div style=\"color: oklch(0.841 0.238 128.85); font-weight: 300;\">"
"		webserv"
"	</div>"
"	{{TEAPOT}}"
"</body>"
"</html>"
};

const std::string	HTTP::m_teapot = {
"<pre style=\"margin-top: 2rem; font-family: font-family: 'Courier New', Courier, monospace; font-size: .5em; color: oklch(0.269 0 0); user-select: none; text-align: left;\">"
"                                                /<br>"
"                                               /<br>"
"                               xxX###xx       /<br>"
"                                ::XXX        /<br>"
"                         xxXX::::::###XXXXXx/#####<br>"
"                    :::XXXXX::::::XXXXXXXXX/    ####<br>"
"         xXXX//::::::://///////:::::::::::/#####    #         ##########<br>"
"      XXXXXX//:::::://///xXXXXXXXXXXXXXXX/#    #######      ###   ###<br>"
"     XXXX        :://///XXXXXXXXX######X/#######      #   ###    #<br>"
"     XXXX        ::////XXXXXXXXX#######/ #     #      ####   #  #<br>"
"      XXXX/:     ::////XXXXXXXXXX#####/  #     #########      ##<br>"
"       \"\"XX//::::::////XXXXXXXXXXXXXX/###########     #       #<br>"
"           \"::::::::////XXXXXXXXXXXX/    #     #     #      ##<br>"
"                 ::::////XXXXXXXXXX/##################   ###<br>"
"                     ::::://XXXXXX/#    #     #   #######<br>"
"                         ::::::::/################<br>"
"                                /<br>"
"                               /<br>"
"                              /"
"</pre>"
};

HTTP::~HTTP()
{
}


/* ************************************************************************** */
/* ************************************************************************** */


std::string HTTP::getStatusMessage(int _status_code)
{
	auto it = m_status_messages.find(_status_code);
	return ( it != m_status_messages.end() ? it->second : std::string("Unknown"));
}

std::string	HTTP::getMimeType(const std::string& path)
{
	size_t	pos = path.find_last_of(".");
	if (pos == std::string::npos || pos == path.length() - 1)
		return ("text/plain");
		
	std::string ext = path.substr(pos);
	auto it = m_mime_types.find(ext);
	return ( it != m_mime_types.end() ? it->second : std::string("application/octet-stream") );
}

std::string	HTTP::getErrorPageTemplate(const int& status_code)
{
	std::string page = m_error_page_template;
	size_t		pos  = 0;

	std::string	needle = "{{STATUS_CODE}}";
	std::string	replacement = std::to_string(status_code);
	while ((pos = page.find(needle, pos)) != std::string::npos) {
		page.replace(pos, needle.length(), replacement);
		pos += replacement.length();
	}

	pos = 0;	
	needle = "{{REASON_PHRASE}}";
	replacement = getStatusMessage(status_code);
	while ((pos = page.find(needle, pos)) != std::string::npos) {
		page.replace(pos, needle.length(), replacement);
		pos += replacement.length();
	}

	pos = 0;	
	needle = "{{TEAPOT}}";
	(status_code == WSSC_I_M_A_TEAPOT)	? replacement = m_teapot
										: replacement = "";
	while ((pos = page.find(needle, pos)) != std::string::npos) {
		page.replace(pos, needle.length(), replacement);
		pos += replacement.length();
	}

	return (page);
}

bool	HTTP::isValidStatusCode(int& statusCode)
{
	if (m_status_messages.find(statusCode) != m_status_messages.end())
		return true;
	return false;
}

std::string	HTTP::methodToString(const HTTP::Method& method)
{
    switch (method)
    {
        case Method::GET:		return "GET";
        case Method::POST:		return "POST";
        case Method::DELETE:	return "DELETE";
        default:				return "UNKNOWN_METHOD";
    }
}

HTTP::Method	HTTP::strToMethod(const std::string& str)
{
	if (str == "GET\0")
		return HTTP::Method::GET;
	else if (str == "POST\0")
		return HTTP::Method::POST;
	else if (str == "DELETE\0")
		return HTTP::Method::DELETE;
	return HTTP::Method::GET;
}
