/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 11:42:37 by nmihaile          #+#    #+#             */
/*   Updated: 2025/02/20 20:11:17 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include "Connection.hpp"

class Server
{
public:
	Server();
	~Server();

	int 				srv_socket;
	struct sockaddr_in	srv_addr;

	void	srv_listen(void);
	
private:
	Server(const Server& other);
	Server&	operator=(const Server& rhs);
};

#endif
