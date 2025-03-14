/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 14:40:05 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/14 18:06:40 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GETHANDLER_HPP
#define GETHANDLER_HPP

#include "AHandler.hpp"
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

	std::string	sanitizePath(const std::string& request_target);
	bool		resourceExist(const std::string& path);
	std::string	fetchContent(const std::string& path);
};

#endif
