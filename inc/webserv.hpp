/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 13:26:18 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/13 18:46:48 by nmihaile         ###   ########.fr       */
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

# if ENABLE_LOG
#  if PRINT_DEBUG
#   define LOG_DEBUG(msg)					Log::debug(msg)
#   define LOG_DEBUG_MV(msg, val)			Log::debug(msg, val)
#  else
#   define LOG_DEBUG(msg)
#   define LOG_DEBUG_MV(msg)
#  endif
#  if PRINT_INFO
#   define LOG_INFO(msg)						Log::info(msg)
#   define LOG_INFO_LM(lab, msg)				Log::info(lab, msg)
#  else
#   define LOG_INFO(msg)
#   define LOG_INFO_LM(lab, msg)
#  endif
#  if PRINT_WARNING
#   define LOG_WARNING(msg)					Log::warning(msg)
#   define LOG_WARNING_LM(lab, msg)			Log::warning(lab, msg)
#  else
#   define LOG_WARNING(msg)
#   define LOG_WARNING_LM(lab, msg)
#  endif
#  if PRINT_ERROR
#   define LOG_ERROR(msg)					Log::error(msg)
#   define LOG_ERROR_LM(lab, msg)			Log::error(lab, msg)
#  else
#   define LOG_ERROR(msg)
#   define LOG_ERROR_LM(lab, msg)
#  endif
#  if PRINT_SUCCESS
#   define LOG_SUCCESS(msg)					Log::success(msg)
#   define LOG_SUCCESS_LM(lab, msg)			Log::success(lab, msg)
#  else
#   define LOG_SUCCESS(msg)
#   define LOG_SUCCESS_LM(lab, msg)
#  endif
#  if PRINT_MSG
#   define LOG_MSG(...)						Log::msg(__VA_ARGS__)
#  else
#   define LOG_MSG(...)
#  endif
#  if PRINT_RAW
#   define LOG_RAW(msg, split)				Log::raw(msg, split)
#  else
#   define LOG_RAW(msg, split)
#  endif
# else
#  define LOG_DEBUG(msg)
#  define LOG_DEBUG_MV(msg, val)
#  define LOG_INFO(msg)
#  define LOG_INFO_LM(lab, msg)
#  define LOG_WARNING(msg)
#  define LOG_WARNING_LM(lab, msg)
#  define LOG_ERROR(msg)
#  define LOG_ERROR_LM(lab, msg)
#  define LOG_SUCCESS(msg)
#  define LOG_SUCCESS_LM(lab, msg)
#  define LOG_MSG(lab, msg, col1, col2)
#  define LOG_RAW(msg, split)
# endif

# include <iostream>
# include <csignal>
# include <string>
# include <unordered_map>

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

void	handleAbort(int signum);
void	trimWhitespaces(std::string& str);
bool	startsWith(const std::string& str, const std::string& prefix);
bool	endsWith(const std::string& str, const std::string& suffix);

#endif