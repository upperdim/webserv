/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 13:26:18 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/11 13:09:50 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_H
# define WEBSERV_H

# define REQUEST_BUFFER_SIZE 1024

# define ENABLE_LOG		true
# define PRINT_DEBUG	true
# define PRINT_INFO		true
# define PRINT_WARNING	true
# define PRINT_ERROR	true
# define PRINT_SUCCESS 	true
# define PRINT_MSG		true
# define PRINT_RAW		true

# include <iostream>
# include <csignal>
# include <string>

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

extern bool	g_running;

void	handleAbort(int signum);
void	trimWhitespaces(std::string& str);
bool	startsWith(const std::string& str, const std::string& prefix);
bool	endsWith(const std::string& str, const std::string& suffix);

#endif