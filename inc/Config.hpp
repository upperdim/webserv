/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/20 16:20:40 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/21 11:03:59 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>

class Config
{
public:
	Config(const std::string _configFilePath);
	~Config();

private:
	Config();
	Config(const Config& other);
	Config&	operator=(const Config& rhs);

	std::string	m_configFilePath;
};

#endif