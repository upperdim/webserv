/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 19:11:36 by nmihaile          #+#    #+#             */
/*   Updated: 2025/02/21 12:59:12 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <sstream>

#include <iostream>

class Connection
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

private:
	Connection();
	Connection(const Connection& other);
	Connection&	operator=(const Connection& rhs);

	bool	m_done;
};

#endif
