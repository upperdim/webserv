/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Log.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 13:14:44 by nmihaile          #+#    #+#             */
/*   Updated: 2025/02/22 13:22:53 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOG_HPP
#define LOG_HPP

#include <string>
#include <iostream>
#include <iomanip>
#include "webserv.hpp"

class Log
{
public:
	~Log();

	static void	debug(const std::string msg);
	static void	debug(const std::string msg, const size_t val);
	static void	info(const std::string msg);
	static void	info(const std::string label, const std::string msg);
	static void	warning(const std::string msg);
	static void	warning(const std::string label, const std::string msg);
	static void	error(const std::string msg);
	static void	error(const std::string label, const std::string msg);
	static void	success(const std::string msg);
	static void	success(const std::string label, const std::string msg);
	static void	msg(const std::string label, const std::string msg, const std::string col1, const std::string col2);
	static void	raw(const std::string msg, size_t split);

	typedef struct s_type
	{
		std::string	name;
		std::string	col1;
		std::string	col2;
	} t_type;

private:
	Log();
	Log(const Log& orher);
	Log&	operator=(const Log& rhs);

	static const size_t DEBUG	= 0;
	static const size_t INFO	= 1;
	static const size_t WARNING	= 2;
	static const size_t ERROR	= 3;
	static const size_t SUCCESS	= 4;

	static const t_type LOG_TYPES[5];

	static void	print(const t_type type, const std::string label, const std::string msg);
};

#endif
