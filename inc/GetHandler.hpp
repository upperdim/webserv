/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 14:40:05 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/18 16:30:01 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GETHANDLER_HPP
#define GETHANDLER_HPP

#include "AHandler.hpp"
#include "Server.hpp"
#include "Response.hpp"
#include "Http.hpp"

class Server;

class GetHandler : public AHandler
{
public:
	GetHandler(const Server& _server);
	virtual ~GetHandler();

	virtual Response	handle(const Request& request);

private:
	GetHandler(const GetHandler& other);
	GetHandler&	operator=(const GetHandler& rhs);

	std::string	sanitizePath(const Request& request, Response& response);
	bool		resourceExist(const std::string& path);
};

#endif
