/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 13:01:20 by haejeong          #+#    #+#             */
/*   Updated: 2024/06/24 19:22:11 by haejeong         ###   ########.fr       */
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
    // configParsing.checkConfigs();
}

#include <set>
#include <sstream>


static std::vector<std::string> split(const std::string &str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::stringstream ss(str);
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

void Webserv::makeServer() {
    configStrings = configParsing.getConfigStrings();
    std::set<std::string> reference;
    
    reference.insert("listen");
    reference.insert("server_name");
    reference.insert("root");
    reference.insert("client_max_body_size");
    reference.insert("index");
    reference.insert("autoindex");
    reference.insert("error_page");
    reference.insert("location");

    for (std::vector<std::string>::iterator it = configStrings.begin(); it != configStrings.end(); it++) {
        std::cout << *it << std::endl;
        std::istringstream temp(*it);
        std::string line;
        while(std::getline(temp, line)) 
        {
            std::vector<std::string> token = split(line, ' ');
            // for (int i = 0; i < token.size(); i++)
            // {
                if (reference.find(token[0]) == reference.end())
                {
                    std::cout << token[0] << std::endl;
                    std::cout << "invalid argument!" << std::endl;
                    exit(1);
                }
            // }
        }
        std::cout << "\n---- new config ----\n" << std::endl;
    }
}