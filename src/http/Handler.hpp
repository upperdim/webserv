/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/26 10:53:29 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/26 10:55:24 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HANDLER_HPP
#define HANDLER_HPP

#include "Request.hpp"
#include "Response.hpp"

class Handler
{
public:
	~Handler();

	static void			handle(Request& request, Response& response);

private:
	Handler();

	static void	handleGetRequest(const Request& request, Response& response);
	static void	handlePostRequest(const Request& request, Response& response);
	static void	handleDeleteRequest(const Request& request, Response& response);
	static void	handleFailedRequest(const Request& request, Response& response);

	static void	createErrorResponse(Response& response, int statusCode);

};

#endif
