/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 13:01:20 by haejeong          #+#    #+#             */
/*   Updated: 2024/06/25 16:51:13 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Webserv.hpp"

Webserv::Webserv() : kq(0) {
	// std::cout << "Default Webserv constructor called" << std::endl;
}

Webserv::~Webserv() {
	// std::cout << "Webserv destructor called" << std::endl;
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

std::vector<ServerConfig> Webserv::getServerConfigs() {
    return (this->configParsing.getServerConfigs());
}

void Webserv::makeServerList() {
    std::vector<ServerConfig> configs;

    configs = this->configParsing.getServerConfigs();
    for (std::vector<ServerConfig>::iterator it = configs.begin(); it != configs.end(); it++) {
        Server serv;
        serv.initServer(*it);
        std::map<int, Server> serverMap;
        serverMap.insert(std::make_pair(serv.getListen(), serv));
    }
}
