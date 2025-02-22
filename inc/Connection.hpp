/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 19:11:36 by nmihaile          #+#    #+#             */
/*   Updated: 2025/02/22 19:49:39 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <sstream>
#include "IEventHandler.hpp"

#include <iostream>

class Connection : public IEventHandler
{
public:

	int			cli_socket;
	std::string	request;
	std::string	response;

	Connection(int _cli_socket);
	~Connection();

	void	recieve(void);
	void	create_response(void);
	void	respond(void);

	virtual void handleReadEvent();
	virtual void handleWriteEvent();
	virtual void handleErrorEvent();

private:
	Connection();
	Connection(const Connection& other);
	Connection&	operator=(const Connection& rhs);

	bool	m_done;
};

#endif
