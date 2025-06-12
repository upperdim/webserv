/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 19:11:45 by tunsal            #+#    #+#             */
/*   Updated: 2025/06/12 18:10:06 by nmihaile         ###   ########.fr       */
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

	print("\n\033[92mserver count: ");
	std::cout << serverBlocks.size();
	printLn("\033[0m\n");

	for (ServerBlock server : serverBlocks) {
		printLn("server {");

		print("\tlisten\t\t\t");
		printLn(std::to_string(server.host) + ":" + std::to_string(server.listenPort) + ";");

		print("\tserver_names\t\t");
		if (server.serverNames.size() == 0)
			print("--");
		else {
			for (const std::string& serverName : server.serverNames) {
				print(serverName + " ");
			}
		}
		printLn(";");

		// Print all ERROR_PAGE directives
		if (server.errorPagePaths.size() > 0) {
			for(std::map<int,std::string>::iterator it = server.errorPagePaths.begin(); it != server.errorPagePaths.end(); ++it)
			{
				print("\terror_page\t\t");
				print(std::to_string(it->first) + " ");
				printLn(it->second);
			}
		}

		print("\tclient_max_body_size\t");
		std::cout << server.clientMaxBodySize;
		printLn(";");

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
