/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 19:12:17 by tunsal            #+#    #+#             */
/*   Updated: 2025/06/09 18:19:05 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>
#include <map>
#include <netinet/in.h>		// for in_addr
#include "Http.hpp"

class LocationBlock
{
public:
	std::string					route;
	size_t 						clientMaxBodySize;
	std::vector<HTTP::Method>	allowMethods;
	std::string					returnRoute;
	std::string					root;
	bool						autoIndex;
	std::string					index;
	std::string					cgiExtension;
	bool						allowUpload;
	std::string					uploadsDir;
};

class ServerBlock
{
public:
	// Subject requirement of this scope
	int							listenPort;
	in_addr_t					host;		// assign in Server constructor like: m_srv_addr.sin_addr.s_addr = config.host;
	std::vector<std::string>	serverNames;
	std::map<int, std::string>	errorPagePaths;
	size_t						clientMaxBodySize;
	
	// Multiscope options
	std::string root;
	std::string index;
};

class Config
{
public:
	Config();
	~Config();

	// Subject requirement of this scope
	std::vector<ServerBlock> serverBlocks;

	// Multiscope options
	size_t						clientMaxBodySize;
	std::string					index;
	std::map<int, std::string>	errorPagePaths;
};

#endif