/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 13:26:18 by nmihaile          #+#    #+#             */
/*   Updated: 2025/06/23 10:29:08 by nmihaile         ###   ########.fr       */
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
# include "colors.hpp"

typedef std::unordered_map<std::string, std::string> HeaderMap;

extern bool	g_running;

void		handleAbort(int signum);
std::string readFile(const std::string& filename);

#endif