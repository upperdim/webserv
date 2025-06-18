/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AHandler.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 14:36:57 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/18 14:38:22 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AHANDLER_HPP
#define AHANDLER_HPP

#include <fstream>
#include "Http.hpp"
#include "Config.hpp"
#include "Request.hpp"
#include "Response.hpp"

class AHandler
{
public:
	AHandler(const ServerBlock& _serverBlock);
	virtual	~AHandler();

	virtual Response	handle(const Request& request) =0;
	virtual	void		createErrorResponse(Response& response, int _status_code);
	virtual std::string	fetchErrorPage(int _status_code) const;

protected:
	const ServerBlock&	m_serverBlock;
};

#endif
