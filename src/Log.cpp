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
	#if ENABLE_LOG && PRINT_INFO
		print(LOG_TYPES[INFO], LOG_TYPES[INFO].name, msg);
	#else
		(void)msg;
	#endif
}

void	Log::info(const std::string label, const std::string msg)
{
	#if ENABLE_LOG && PRINT_INFO
		print(LOG_TYPES[INFO], label, msg);
	#else
		(void)label;
		(void)msg;
	#endif
}

void	Log::warning(const std::string msg)
{
	#if ENABLE_LOG && PRINT_WARNING
		print(LOG_TYPES[WARNING], LOG_TYPES[WARNING].name, msg);
	#else
		(void)msg;
	#endif

}

void	Log::warning(const std::string label, const std::string msg)
{
	#if ENABLE_LOG && PRINT_WARNING
		print(LOG_TYPES[WARNING], label, msg);
	#else
		(void)label;
		(void)msg;
	#endif
}

void	Log::error(const std::string msg)
{
	#if ENABLE_LOG && PRINT_ERROR
		print(LOG_TYPES[ERROR], LOG_TYPES[ERROR].name, msg);
	#else
		(void)msg;
	#endif
}

void	Log::error(const std::string label, const std::string msg)
{
	#if ENABLE_LOG && PRINT_ERROR
		print(LOG_TYPES[ERROR], label, msg);
	#else
		(void)label;
		(void)msg;
	#endif
}

void	Log::success(const std::string msg)
{
	#if ENABLE_LOG && PRINT_SUCCESS
		print(LOG_TYPES[SUCCESS], LOG_TYPES[SUCCESS].name, msg);
	#else
		(void)msg;
	#endif
}

void	Log::success(const std::string label, const std::string msg)
{
	#if ENABLE_LOG && PRINT_SUCCESS
		print(LOG_TYPES[SUCCESS], label, msg);
	#else
		(void)label;
		(void)msg;
	#endif
}

void	Log::msg(const std::string label, const std::string msg, const std::string col1, const std::string col2)
{
	#if ENABLE_LOG && PRINT_MSG
		std::cerr	<< col1 << BOLD << label 
					<< col2 << REGULAR << msg
					<< RESET << std::endl;
	#else
		(void)label;
		(void)msg;
		(void)col1;
		(void)col2;
	#endif
}

void	Log::raw(const std::string msg, size_t split)
{
	#if ENABLE_LOG && PRINT_RAW
		std::cerr	<< DEFAULT << BOLD << "[RAW]";
		std::cerr	<< std::hex << std::setfill('0');

		size_t c = 0;

		for (auto it = msg.begin(); it < msg.end(); ++it)
		{
			std::isprint(*it)	? std::cerr << DEFAULT	: std::cerr << LIGHTMAGENTA;
			(c == 0)			? std::cerr << "\n"		: std::cerr << " ";
			std::cerr << std::setw(2) << static_cast<unsigned int>(*it);
			if (split == 0)
				(*it == '\n') ? c = 0 : ++c;
			else
				(*it == '\n' || c == split - 1) ? c = 0 : ++c;
		}
		std::cerr << std::endl;
	#else
		(void)msg;
		(void)split;
	#endif
}

void	Log::print(const t_type type, const std::string label, const std::string msg)
{
	std::cerr	<< type.col1 << BOLD << "[" << label << "] "
				<< type.col2 << REGULAR << msg
				<< RESET << std::endl;
}
