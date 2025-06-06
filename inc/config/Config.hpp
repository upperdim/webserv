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
	size_t clientMaxBodySize;   // is once per config enough to satisfy the subject?
	std::vector<HTTP::Method> methods;
	std::string returnLocation; // redirect, TODO: idk in which form, uri? path?
	std::string root;
	bool autoIndex;             // directory listing
	std::string index;          // subject doesn't refer to this as plural?
	std::string cgiExtension;   // subject doesn't refer to this as plural?
	//bool allowUpload;         // use POST?
	std::string uploadsDir;
};

class ServerBlock
{
public:
	int listenPort;
	std::string host;
	std::vector<std::string> serverNames;
	std::map<int, std::string> errorPagePaths;
	size_t clientMaxBodySize;
};

class Config
{
public:
	Config();
	~Config();



private:
};

#endif