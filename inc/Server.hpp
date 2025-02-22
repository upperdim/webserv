/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 11:42:37 by nmihaile          #+#    #+#             */
/*   Updated: 2025/02/22 19:56:26 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include "IEventHandler.hpp"
#include "Connection.hpp"


class Server : public IEventHandler
{
public:
	Server();
	~Server();

	struct sockaddr_in	srv_addr;

	void	srv_listen(void);

	virtual void handleReadEvent();
	virtual void handleWriteEvent();
	virtual void handleErrorEvent();

private:
	Server(const Server& other);
	Server&	operator=(const Server& rhs);
};

#endif
