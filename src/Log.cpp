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
	print(LOG_TYPES[DEBUG], LOG_TYPES[DEBUG].name, msg);
}

void	Log::debug(const std::string msg, const size_t val)
{
	print(LOG_TYPES[DEBUG], LOG_TYPES[DEBUG].name, msg + std::to_string(val));
}

void	Log::info(const std::string msg)
{
	print(LOG_TYPES[INFO], LOG_TYPES[INFO].name, msg);
}

void	Log::info(const std::string label, const std::string msg)
{
	print(LOG_TYPES[INFO], label, msg);
}

void	Log::warning(const std::string msg)
{
	print(LOG_TYPES[WARNING], LOG_TYPES[WARNING].name, msg);
}

void	Log::warning(const std::string label, const std::string msg)
{
	print(LOG_TYPES[WARNING], label, msg);
}

void	Log::error(const std::string msg)
{
	print(LOG_TYPES[ERROR], LOG_TYPES[ERROR].name, msg);
}

void	Log::error(const std::string label, const std::string msg)
{
	print(LOG_TYPES[ERROR], label, msg);
}

void	Log::success(const std::string msg)
{
	print(LOG_TYPES[SUCCESS], LOG_TYPES[SUCCESS].name, msg);
}

void	Log::success(const std::string label, const std::string msg)
{
	print(LOG_TYPES[SUCCESS], label, msg);
}

void	Log::msg(const std::string label, const std::string msg, const std::string col1, const std::string col2)
{
	std::cerr	<< col1 << BOLD << label 
				<< col2 << REGULAR << msg
				<< RESET << std::endl;
}

void	Log::print(const t_type type, const std::string label, const std::string msg)
{
	std::cerr	<< type.col1 << BOLD << "[" << label << "] "
				<< type.col2 << REGULAR << msg
				<< RESET << std::endl;
}
