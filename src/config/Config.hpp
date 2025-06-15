/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 19:12:17 by tunsal            #+#    #+#             */
/*   Updated: 2025/06/15 11:00:35 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>			// TODO: delete me if we delete the print functions
#include <string>
#include <vector>
#include <map>
#include <netinet/in.h>		// for in_addr
#include "Http.hpp"
#include "colors.hpp"

class LocationBlock
{
public:
	std::string					route;				//	path after location
	size_t 						clientMaxBodySize;	//	::	client_max_body_size
	std::vector<HTTP::Method>	allowMethods;		//	::	allow_methods
	std::string					returnRoute;		//	::	return		TBD: should we name it: redirect
	std::string					root;				//	::	root
	bool						autoIndex;			//	::	autoindex
	std::string					index;				//	::	index
	std::string					cgiExtension;		//	::	cgi_extension
	bool						allowUpload;		//	::	allow_upload
	std::string					uploadDir;			//	::	upload_store
};

class ServerBlock
{
public:
	// Subject requirement of this scope
	int							listenPort         = 80;					//	::	listen
	in_addr_t					host               = 0;						//	assign in Server constructor like: m_srv_addr.sin_addr.s_addr = config.host;
	std::vector<std::string>	serverNames;								//	::	server_name
	std::map<int, std::string>	errorPagePaths;								//	::	error_page
	size_t						clientMaxBodySize  = 1073741824; // 1 MB	//	::	client_max_body_size
	std::vector<LocationBlock>	locationsBlocks;

	// Multiscope options
	std::string root;								//	::	root
	std::string index = "index.html";				//	::	index
};

class Config
{
public:
	Config();
	~Config();

	// Subject requirement of this scope
	std::vector<ServerBlock>	serverBlocks;		//	::	server

	// Multiscope options
	size_t						clientMaxBodySize;	//	::	client_max_body_size
	std::map<int, std::string>	errorPagePaths;		//	::	error_page
	std::string					index;				//	::	index

	void	printConfigs(void) const;				// TODO: delete me 

private:
	void	print(const std::string& str) const;	// TODO: delete me 
	void	printLn(const std::string& str) const;	// TODO: delete me 
};

#endif