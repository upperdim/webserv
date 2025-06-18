/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 19:11:45 by tunsal            #+#    #+#             */
/*   Updated: 2025/06/18 12:10:28 by nmihaile         ###   ########.fr       */
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


void	Config::printConfigs(void) const
{

	printLn(LIGHTGREEN"\nPRINT CONFIG");
	print(LIGHTGREEN"SERVERS: ");
	std::cout << LIGHTMAGENTA << serverBlocks.size();
	printLn("");

	for (ServerBlock server : serverBlocks) {
		printLn(LIGHTCYAN"server" LIGHTYELLOW " {" RESET);

		print(LIGHTCYAN"\tlisten\t\t\t" RESET);
		print(LIGHTMAGENTA + std::to_string(server.listenHost) + ":" + std::to_string(server.listenPort));
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
			for (std::map<int,std::string>::iterator it = server.errorPagePaths.begin(); it != server.errorPagePaths.end(); ++it)
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

		// print locations
		for (LocationBlock location : server.locationBlocks) {
			printLn(LIGHTCYAN"\n\tlocation " LIGHTMAGENTA + location.route +  LIGHTYELLOW " {" RESET);

			if (!location.returnRoute.empty()) {
				print(LIGHTCYAN"\t\treturn\t\t\t" RESET);
				print(MAGENTA " " + location.returnRoute);
				printLn(GRAY";" RESET);
			}

			print(LIGHTCYAN"\t\tautoindex\t\t" RESET);
			location.autoIndex ? print(LIGHTMAGENTA "on") : print(LIGHTMAGENTA "off");
			printLn(GRAY";" RESET);

			print(LIGHTCYAN"\t\tallow_upload\t\t" RESET);
			location.allowUpload ? print(LIGHTMAGENTA "on") : print(LIGHTMAGENTA "off");
			printLn(GRAY";" RESET);

			print(LIGHTCYAN"\t\tupload_store\t\t" RESET);
			print(LIGHTMAGENTA + location.uploadDir);
			printLn(GRAY";" RESET);

			print(LIGHTCYAN"\t\tcgi_extension\t\t" RESET);
			print(LIGHTMAGENTA + location.cgiExtension);
			printLn(GRAY";" RESET);

			if (location.allowMethods.size() > 0) {
				print(LIGHTCYAN"\t\tallow_methods\t\t" RESET);
				for (HTTP::Method method : location.allowMethods)
				{
					switch (method) {
						case HTTP::Method::GET:		print(LIGHTMAGENTA"GET"); break;
						case HTTP::Method::POST:	print(LIGHTMAGENTA"POST"); break;
						case HTTP::Method::DELETE:	print(LIGHTMAGENTA"DELETE"); break;
						default:					print(LIGHTMAGENTA"INVALID_METHOD"); break;
					}
					if (method != location.allowMethods.back()) 
						print(" ");
				}
				printLn(GRAY";" RESET);
			}

			print(LIGHTCYAN"\t\tclient_max_body_size\t" RESET);
			std::cout << LIGHTMAGENTA << location.clientMaxBodySize;
			printLn(GRAY";" RESET);

			print(LIGHTCYAN"\t\troot\t\t\t" RESET);
			print(LIGHTMAGENTA + location.root);
			printLn(GRAY";" RESET);

			print(LIGHTCYAN"\t\tindex\t\t\t" RESET);
			print(LIGHTMAGENTA + location.index);
			printLn(GRAY";" RESET);

			print(LIGHTYELLOW"\t}\n" RESET);
		}

		print(LIGHTYELLOW"}\n" RESET);
	}
}


/* ************************************************************************** */
/* ************************************************************************** */


void	Config::print(const std::string& str) const
{
	std::cout << str;
}

void	Config::printLn(const std::string& str) const
{
	print(str);
	print("\n");
}
