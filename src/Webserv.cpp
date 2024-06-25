/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 13:01:20 by haejeong          #+#    #+#             */
/*   Updated: 2024/06/25 15:54:32 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Webserv.hpp"

Webserv::Webserv() : kq(0) {
	std::cout << "Default Webserv constructor called" << std::endl;
}

Webserv::~Webserv() {
	std::cout << "Webserv destructor called" << std::endl;
}

void Webserv::makeServerConfigStringList(const std::string & configPath) {
	std::ifstream configFile(configPath.c_str());
    if (!configFile.is_open()) {
        throw RuntimeException("Invalid configuration file path");
    }
	std::stringstream fileBuffer;
    fileBuffer << configFile.rdbuf();
    configFile.close();
    std::string fileInput = fileBuffer.str();
	configParsing.removeComment(fileInput);
    configParsing.removeEmptyLine(fileInput);
    configParsing.splitServer(fileInput);
    configParsing.setServerConfig();
}

static std::vector<std::string> split(const std::string &str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::stringstream ss(str);
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}