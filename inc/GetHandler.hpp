/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 14:40:05 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/11 13:06:14 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GETHANDLER_HPP
#define GETHANDLER_HPP

#include <fstream>
#include "AHandler.hpp"
#include "Http.hpp"

class GetHandler : public AHandler
{
public:
	GetHandler();
	virtual ~GetHandler();

	virtual Response	handle(const Request& request);

private:
	GetHandler(const GetHandler& other);
	GetHandler&	operator=(const GetHandler& rhs);

	void		createResponseData(std::string& _data, const Request& request);
	std::string	statusLine(const Request& request);
	std::string	headers(const Request& request);
	std::string	sanitizePath(const std::string& request_target);
};

#endif
