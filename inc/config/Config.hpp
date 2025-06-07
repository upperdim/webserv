/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tunsal <tunsal@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 19:12:17 by tunsal            #+#    #+#             */
/*   Updated: 2025/06/06 19:12:17 by tunsal           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>
#include <map>
#include "Http.hpp"

class LocationBlock
{
public:
	std::string route;
	size_t clientMaxBodySize;
	std::vector<HTTP::Method> limitExcept;
	std::string returnRoute;
	std::string root;
	bool autoIndex;
	std::string index;
	std::string cgiExtension;
	//bool allowUpload;
	std::string uploadsDir;
};

class ServerBlock
{
public:
	// Subject requirement of this scope
	int listenPort;
	std::string host;
	std::vector<std::string> serverNames;
	std::map<int, std::string> errorPagePaths;
	size_t clientMaxBodySize;
	
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
	size_t clientMaxBodySize;
	std::string index;
	std::map<int, std::string> errorPagePaths;
};

#endif