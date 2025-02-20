/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 11:42:37 by nmihaile          #+#    #+#             */
/*   Updated: 2025/02/20 14:56:52 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>

class Server
{
public:
	Server();
	~Server();

	int 				srv_socket;
	struct sockaddr_in	srv_addr;

	void	srv_listen(void);
	void	srv_respond(int cli_socket, std::string response);
	void	srv_close(int cli_socket);
	
private:
	Server(const Server& other);
	Server&	operator=(const Server& rhs);
};

#endif
