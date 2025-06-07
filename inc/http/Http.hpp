/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 20:09:01 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/13 16:56:25 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_HPP
#define HTTP_HPP

#define	WSSC_CONTINUE							100
#define	WSSC_SWITCHING_PROTOCOLS				101
#define	WSSC_PROCESSING							102
#define	WSSC_EARLY_HINTS						103

#define	WSSC_OK									200
#define	WSSC_CREATED							201
#define	WSSC_ACCEPTED							202
#define	WSSC_NON_AUTHORITATIVE_INFORMATION		203
#define	WSSC_NO_CONTENT							204
#define	WSSC_RESET_CONTENT						205
#define	WSSC_PARTIAL_CONTENT					206
#define	WSSC_MULTI_STATUS						207
#define	WSSC_ALREADY_REPORTED					208
#define	WSSC_IM_USED							226

#define	WSSC_MULTIPLE_CHOICES					300
#define	WSSC_MOVED_PERMANENTLY					301
#define	WSSC_FOUND								302
#define	WSSC_SEE_OTHER							303
#define	WSSC_NOT_MODIFIED						304
#define	WSSC_TEMPORARY_REDIRECT					307
#define	WSSC_PERMANENT_REDIRECT					308

#define	WSSC_BAD_REQUEST						400
#define	WSSC_UNAUTHORIZED						401
#define	WSSC_PAYMENT_REQUIRED					402
#define	WSSC_FORBIDDEN							403
#define	WSSC_NOT_FOUND							404
#define	WSSC_METHOD_NOT_ALLOWED					405
#define	WSSC_NOT_ACCEPTABLE						406
#define	WSSC_PROXY_AUTHENTICATION_REQUIRED		407
#define	WSSC_REQUEST_TIMEOUT					408
#define	WSSC_CONFLICT							409
#define	WSSC_GONE								410
#define	WSSC_LENGTH_REQUIRED					411
#define	WSSC_PRECONDITION_FAILED				412
#define	WSSC_CONTENT_TOO_LARGE					413
#define	WSSC_URI_TOO_LONG						414
#define	WSSC_UNSUPPORTED_MEDIA_TYPE				415
#define	WSSC_RANGE_NOT_SATISFIABLE				416
#define	WSSC_EXPECTATION_FAILED					417
#define	WSSC_I_M_A_TEAPOT						418
#define	WSSC_MISDIRECTED_REQUEST				421
#define	WSSC_UNPROCESSABLE_CONTENT				422
#define	WSSC_LOCKED								423
#define	WSSC_FAILED_DEPENDENCY					424
#define	WSSC_TOO_EARLY							425
#define	WSSC_UPGRADE_REQUIRED					426
#define	WSSC_PRECONDITION_REQUIRED				428
#define	WSSC_TOO_MANY_REQUESTS					429
#define	WSSC_REQUEST_HEADER_FIELDS_TOO_LARGE	431
#define	WSSC_UNAVAILABLE_FOR_LEGAL_REASONS		451

#define	WSSC_INTERNAL_SERVER_ERROR				500
#define	WSSC_NOT_IMPLEMENTED					501
#define	WSSC_BAD_GATEWAY						502
#define	WSSC_SERVICE_UNAVAILABLE				503
#define	WSSC_GATEWAY_TIMEOUT					504
#define	WSSC_HTTP_VERSION_NOT_SUPPORTED			505
#define	WSSC_VARIANT_ALSO_NEGOTIATES			506
#define	WSSC_INSUFFICIENT_STORAGE				507
#define	WSSC_LOOP_DETECTED						508
#define	WSSC_NOT_EXTENDED						510
#define	WSSC_NETWORK_AUTHENTICATION_REQUIRED	511

#include <string>
#include <map>

class HTTP
{
public:
	enum Method
	{
		GET,
		POST,
		DELETE
	};

	~HTTP();


	static std::string	getStatusMessage(int _status_code);
	static std::string	getMimeType(const std::string& path);
	static std::string	getErrorPageTemplate(const int& status_code);

private:
	static const std::map<int, std::string>			m_status_messages;
	static const std::map<std::string, std::string>	m_mime_types;
	static const std::string						m_error_page_template;
	static const std::string						m_teapot;

	HTTP();
	HTTP(const HTTP& other);
	HTTP&	operator=(const HTTP& rhs);
};

#endif
