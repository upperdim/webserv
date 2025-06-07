/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tunsal <tunsal@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 17:56:59 by tunsal            #+#    #+#             */
/*   Updated: 2025/06/06 17:56:59 by tunsal           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "Parser.hpp"
#include "Config.hpp"

Config Parser::mockParseConfig(std::string configFilePath) {
	std::string configFile = readConfigFile(configFilePath);

	Config config;
	std::cout << configFile << std::endl;
	return config;
}

std::string Parser::readConfigFile(std::string configFilePath) {
	std::ifstream configFile(configFilePath);
	if (!configFile) {
		throw std::runtime_error("Config file is not found.");
	}

	std::stringstream buffer;
	buffer << configFile.rdbuf();
	configFile.close();

	return buffer.str();
}
