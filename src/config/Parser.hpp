/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tunsal <tunsal@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 17:50:21 by tunsal            #+#    #+#             */
/*   Updated: 2025/06/06 17:50:21 by tunsal           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include "Config.hpp"

class Parser
{
public:
	Config mockParseConfig(std::string configFilePath);
private:
	std::string readConfigFile(std::string configFilePath);
};

#endif