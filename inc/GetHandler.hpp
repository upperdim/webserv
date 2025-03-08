/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 14:40:05 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/08 14:14:51 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GETHANDLER_HPP
#define GETHANDLER_HPP

#include "IHandler.hpp"
#include "StatusCodes.hpp"

class GetHandler : public IHandler
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
};

#endif
