/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 19:11:45 by tunsal            #+#    #+#             */
/*   Updated: 2025/06/15 11:22:28 by nmihaile         ###   ########.fr       */
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

	printLn(LIGHTGREEN"\nPRINT CONFIG");
	print(LIGHTGREEN"SERVERS: ");
	std::cout << LIGHTMAGENTA << serverBlocks.size();
	printLn("");

	for (ServerBlock server : serverBlocks) {
		printLn(LIGHTCYAN"server" LIGHTYELLOW " {" RESET);

		print(LIGHTCYAN"\tlisten\t\t\t" RESET);
		print(LIGHTMAGENTA + std::to_string(server.host) + ":" + std::to_string(server.listenPort));
		printLn(GRAY";" RESET);

		print(LIGHTCYAN"\tserver_names\t\t" RESET);
		if (server.serverNames.size() == 0)
			print(LIGHTMAGENTA"--" RESET);
		else {
			for (const std::string& serverName : server.serverNames) {
				print(LIGHTMAGENTA + serverName);
				if (serverName != server.serverNames.back())
					print(" ");
			}
		}
		printLn(GRAY";" RESET);

		// Print all ERROR_PAGE directives
		if (server.errorPagePaths.size() > 0) {
			for(std::map<int,std::string>::iterator it = server.errorPagePaths.begin(); it != server.errorPagePaths.end(); ++it)
			{
				print(LIGHTCYAN"\terror_page\t\t" RESET);
				print(LIGHTMAGENTA + std::to_string(it->first) + " ");
				print(LIGHTBLUE + it->second);
				printLn(GRAY";" RESET);
			}
		}

		print(LIGHTCYAN"\tclient_max_body_size\t" RESET);
		std::cout << LIGHTMAGENTA << server.clientMaxBodySize;
		printLn(GRAY";" RESET);

		print(LIGHTCYAN"\troot\t" RESET);
		print(LIGHTMAGENTA "\t\t" + server.root);
		printLn(GRAY";" RESET);

		print(LIGHTCYAN"\tindex\t" RESET);
		print(LIGHTMAGENTA "\t\t" + server.index);
		printLn(GRAY";" RESET);

		print(LIGHTYELLOW"}\n" RESET);
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
