/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StatusCodes.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 20:09:01 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/02 20:31:13 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STATUSCODES_HPP
#define STATUSCODES_HPP

#define	WBSC_CONTINUE							100
#define	WBSC_SWITCHING_PROTOCOLS				101
#define	WBSC_PROCESSING							102
#define	WBSC_EARLY_HINTS						103

#define	WBSC_OK									200
#define	WBSC_CREATED							201
#define	WBSC_ACCEPTED							202
#define	WBSC_NON_AUTHORITATIVE_INFORMATION		203
#define	WBSC_NO_CONTENT							204
#define	WBSC_RESET_CONTENT						205
#define	WBSC_PARTIAL_CONTENT					206
#define	WBSC_MULTI_STATUS						207
#define	WBSC_ALREADY_REPORTED					208
#define	WBSC_IM_USED							226

#define	WBSC_MULTIPLE_CHOICES					300
#define	WBSC_MOVED_PERMANENTLY					301
#define	WBSC_FOUND								302
#define	WBSC_SEE_OTHER							303
#define	WBSC_NOT_MODIFIED						304
#define	WBSC_TEMPORARY_REDIRECT					307
#define	WBSC_PERMANENT_REDIRECT					308

#define	WBSC_BAD_REQUEST						400
#define	WBSC_UNAUTHORIZED						401
#define	WBSC_PAYMENT_REQUIRED					402
#define	WBSC_FORBIDDEN							403
#define	WBSC_NOT_FOUND							404
#define	WBSC_METHOD_NOT_ALLOWED					405
#define	WBSC_NOT_ACCEPTABLE						406
#define	WBSC_PROXY_AUTHENTICATION_REQUIRED		407
#define	WBSC_REQUEST_TIMEOUT					408
#define	WBSC_CONFLICT							409
#define	WBSC_GONE								410
#define	WBSC_LENGTH_REQUIRED					411
#define	WBSC_PRECONDITION_FAILED				412
#define	WBSC_CONTENT_TOO_LARGE					413
#define	WBSC_URI_TOO_LONG						414
#define	WBSC_UNSUPPORTED_MEDIA_TYPE				415
#define	WBSC_RANGE_NOT_SATISFIABLE				416
#define	WBSC_EXPECTATION_FAILED					417
#define	WBSC_I_M_A_TEAPOT						418
#define	WBSC_MISDIRECTED_REQUEST				421
#define	WBSC_UNPROCESSABLE_CONTENT				422
#define	WBSC_LOCKED								423
#define	WBSC_FAILED_DEPENDENCY					424
#define	WBSC_TOO_EARLY							425
#define	WBSC_UPGRADE_REQUIRED					426
#define	WBSC_PRECONDITION_REQUIRED				428
#define	WBSC_TOO_MANY_REQUESTS					429
#define	WBSC_REQUEST_HEADER_FIELDS_TOO_LARGE	431
#define	WBSC_UNAVAILABLE_FOR_LEGAL_REASONS		451

#define	WBSC_INTERNAL_SERVER_ERROR				500
#define	WBSC_NOT_IMPLEMENTED					501
#define	WBSC_BAD_GATEWAY						502
#define	WBSC_SERVICE_UNAVAILABLE				503
#define	WBSC_GATEWAY_TIMEOUT					504
#define	WBSC_HTTP_VERSION_NOT_SUPPORTED			505
#define	WBSC_VARIANT_ALSO_NEGOTIATES			506
#define	WBSC_INSUFFICIENT_STORAGE				507
#define	WBSC_LOOP_DETECTED						508
#define	WBSC_NOT_EXTENDED						510
#define	WBSC_NETWORK_AUTHENTICATION_REQUIRED	511

#endif
