/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Log.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 13:16:39 by nmihaile          #+#    #+#             */
/*   Updated: 2025/02/22 13:22:51 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Log.hpp"

Log::~Log()
{
}

const Log::t_type Log::LOG_TYPES[5] = {
	{std::string("DEBUG"), RESET, RESET},
	{std::string("INFO"), LIGHTBLUE, BLUE},
	{std::string("WARNING"), LIGHTYELLOW, YELLOW},
	{std::string("ERROR"), LIGHTRED, RED},
	{std::string("SUCCESS"), LIGHTGREEN, GREEN}
};


/* ************************************************************************** */
/* ************************************************************************** */


void	Log::debug(const std::string msg)
{
	print(LOG_TYPES[DEBUG], msg);
}

void	Log::info(const std::string msg)
{
	print(LOG_TYPES[INFO], msg);
}

void	Log::warning(const std::string msg)
{
	print(LOG_TYPES[WARNING], msg);
}

void	Log::error(const std::string msg)
{
	print(LOG_TYPES[ERROR], msg);
}

void	Log::success(const std::string msg)
{
	print(LOG_TYPES[SUCCESS], msg);
}

void	Log::msg(const std::string label, const std::string msg, const std::string col1, const std::string col2)
{
	std::cerr	<< col1 << BOLD << label 
				<< col2 << REGULAR << msg
				<< RESET << std::endl;
}

void	Log::print(const t_type type, const std::string msg)
{
	std::cerr	<< type.col1 << BOLD << "[" << type.name << "] "
				<< type.col2 << REGULAR << msg
				<< RESET << std::endl;
}
