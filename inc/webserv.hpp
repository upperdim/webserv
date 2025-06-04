/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 13:26:18 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/04 15:07:47 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_H
# define WEBSERV_H

# define REQUEST_BUFFER_SIZE 1024
// a good default RESPONSE_BUFFER_SIZE seams to be 64kb - 128kb
# define RESPONSE_BUFFER_SIZE (64 * 1024)

# include <iostream>
# include <csignal>
# include <string>
# include <unordered_map>
# include <fstream>
# include <sstream>
# include <iostream>
# include "Log.hpp"

# define BOLD			"\033[1m"
# define REGULAR		"\033[22m"
# define RED			"\033[31m"
# define GREEN			"\033[32m"
# define YELLOW			"\033[33m"
# define BLUE			"\033[34m"
# define MAGENTA		"\033[35m"
# define CYAN			"\033[36m"
# define LIGHTRED		"\033[91m"
# define LIGHTGREEN		"\033[92m"
# define LIGHTYELLOW	"\033[93m"
# define LIGHTBLUE		"\033[94m"
# define LIGHTMAGENTA	"\033[95m"
# define LIGHTCYAN		"\033[96m"
# define RESET			"\033[0m"
# define DEFAULT		"\033[0m"

typedef std::unordered_map<std::string, std::string> HeaderMap;

extern bool	g_running;

void		handleAbort(int signum);
void		trimWhitespaces(std::string& str);
bool		startsWith(const std::string& str, const std::string& prefix);
bool		endsWith(const std::string& str, const std::string& suffix);
std::string readFile(const std::string& filename);

#endif