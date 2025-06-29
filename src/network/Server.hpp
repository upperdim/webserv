/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tunsal <tunsal@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 02:25:20 by tunsal            #+#    #+#             */
/*   Updated: 2025/06/29 03:27:46 by tunsal           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <netinet/in.h>
#include "Config.hpp"

#define LISTEN_BACKLOG 10 // Maximum number of connection requests in queue

class Server
{
private:
	int					fd; 		// File descriptor of the Server socket
	struct sockaddr_in	sockaddr;
	ServerBlock			serverBlock;

public:
	Server(ServerBlock serverBlock);
	
	int					getFd();
	ServerBlock&		getServerBlockRef();
};

#endif
