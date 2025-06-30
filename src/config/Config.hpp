/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 19:12:17 by tunsal            #+#    #+#             */
/*   Updated: 2025/06/30 17:04:28 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <netinet/in.h>		// for in_addr
#include "HTTP.hpp"
#include "colors.hpp"

class LocationBlock
{
public:
	std::string					route;										//	path after location
	std::vector<HTTP::Method>	allowMethods	= {HTTP::Method::GET};		//	::	allow_methods
	std::string					returnRoute;
	bool						autoIndex		= false;					//	::	autoindex
	std::string					cgiExtension;								//	::	cgi_extension
	bool						allowUpload		= false;					//	::	allow_upload
	std::string					uploadDir;									//	::	upload_store

	// Multiscope options
	size_t 						clientMaxBodySize;	//	::	client_max_body_size
	std::string					index;				//	::	index
	std::string					root;				//	::	root
};

class ServerBlock
{
public:
	// Subject requirement of this scope
	int							listenPort         = 80;					//	::	listen
	std::string					listenHostStr;								//		will be set through parseListenDirective in Parser
	std::vector<std::string>	serverNames;								//	::	server_name
	std::map<int, std::string>	errorPagePaths;								//	::	error_page
	size_t						clientMaxBodySize  = 1048576; // 1 MB		//	::	client_max_body_size
	std::vector<LocationBlock>	locationBlocks;

	// Multiscope options
	std::string root;								//	::	root
	std::string index;								//	::	index
};

class Config
{
public:
	Config();
	~Config();

	struct {
		std::string	root;
		std::string	index;
		
		// unused for now
		size_t						clientMaxBodySize;	//	TODO: set type to ssize_t
		std::vector<HTTP::Method>	allowMethods;
		bool						autoIndex;
		bool						allowUpload;
	} fallback;

	// Subject requirement of this scope
	std::vector<ServerBlock>	serverBlocks;		//	::	server

	void	printConfigs(void) const;

private:
	void	print(const std::string& str) const;
	void	printLn(const std::string& str) const;
};

#endif