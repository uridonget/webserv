/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 13:01:20 by haejeong          #+#    #+#             */
/*   Updated: 2024/06/27 11:27:23 by haejeong         ###   ########.fr       */
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
        EV_SET(&changeList[0], serv.getServerFd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
        std::map<int, Server> serverMap;
        serverMap.insert(std::make_pair(serv.getListen(), serv));
    }
}

void Webserv::runServers() {
    for (std::map<int, Server>::iterator it = serverList.begin(); it != serverList.end(); it++) {
        // it->second.runServer();
    }
}

void Webserv::initKqueue() {
    kq = kqueue();
    if (kq < 0) {
        throw RuntimeException("kqueue");
    }
}

void Webserv::connectKqueueToServer() {
    
}