/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 19:11:45 by tunsal            #+#    #+#             */
/*   Updated: 2025/06/11 20:39:34 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config()
{
}

Config::~Config()
{
}

/* ************************************************************************** */
/* ************************************************************************** */



void	Config::printConfigs(void) const	// TODO: delete me 
{
	for (ServerBlock server : serverBlocks) {
		printLn("server {");

		print("\tlisten\t\t");
		printLn(std::to_string(server.host) + ":" + std::to_string(server.listenPort) + ";");

		print("\tserver_names\t");
		if (server.serverNames.size() == 0)
			print("--");
		else {
			for (const std::string& serverName : server.serverNames) {
				print(serverName + " ");
			}
		}
		printLn(";");

		// Print alle ERROR_PAGE directives
		if (server.errorPagePaths.size() > 0) {
			for(std::map<int,std::string>::iterator it = server.errorPagePaths.begin(); it != server.errorPagePaths.end(); ++it)
			{
				print("\terror_page\t");
				print(std::to_string(it->first) + " ");
				printLn(it->second);
			}
		}

		print("}\n");
	}
}


/* ************************************************************************** */
/* ************************************************************************** */


void	Config::print(const std::string& str) const	// TODO: delete me 
{
	std::cout << str;
}

void	Config::printLn(const std::string& str) const	// TODO: delete me 
{
	print(str);
	print("\n");
}
