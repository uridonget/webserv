/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 14:31:23 by haejeong          #+#    #+#             */
/*   Updated: 2024/06/25 16:51:19 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ServerConfig.hpp"

ServerConfig::ServerConfig()
    : listen(0),
    serverName(""),
    root("/"),
    clientMaxBodySize(0),
    index("index.html"),
    redirection("") {
    // std::cout << "ServerConfig default constructor called" << std::endl;
}

ServerConfig::~ServerConfig() {
	// std::cout << "ServerConfig destructor called" << std::endl;
}

void ServerConfig::parseConfig(const std::string& configStr) {
    std::istringstream iss(configStr);
    std::string line;

    Location currentLocation;
    bool inLocationBlock = false;

    while (std::getline(iss, line)) {
        std::istringstream lineStream(line);
        std::string key;
        lineStream >> key;

        if (key == "listen") {
            int port;
            lineStream >> port;
            listen = port;
        } else if (key == "server_name") {
            lineStream >> serverName;
        } else if (key == "root") {
            lineStream >> root;
        } else if (key == "client_max_body_size") {
            lineStream >> clientMaxBodySize;
        } else if (key == "index") {
            lineStream >> index;
        } else if (key == "error_page") {
            int errorCode;
            std::string errorPath;
            lineStream >> errorCode >> errorPath;
            errorPages[errorCode] = errorPath;
        } else if (key == "return") {
            int code;
            std::string path;
            lineStream >> code >> path;
            redirection = path;
        } else if (key == "location") {
            if (inLocationBlock) {
                locationList.push_back(currentLocation);
                currentLocation = Location();
            }
            std::string locationPath;
            lineStream >> locationPath;
            currentLocation.setPath(locationPath);
            inLocationBlock = true;
        } else if (key == "autoindex") {
            std::string value;
            lineStream >> value;
            currentLocation.setAutoIndex(value == "on");
        } else if (key == "index" && inLocationBlock) {
            std::string idx;
            lineStream >> idx;
            currentLocation.setIndex(idx);
        } else if (key == "root" && inLocationBlock) {
            std::string rt;
            lineStream >> rt;
            currentLocation.setRoot(rt);
        } else if (key == "}") {
            if (inLocationBlock) {
                locationList.push_back(currentLocation);
                currentLocation = Location();
                inLocationBlock = false;
            }
        }
    }
    if (inLocationBlock) {
        locationList.push_back(currentLocation);
    }
}

void ServerConfig::printConfig() {
    std::cout << "Listen: " << listen << std::endl;
    std::cout << "Server Name: " << serverName << std::endl;
    std::cout << "Root: " << root << std::endl;
    std::cout << "Index: " << index << std::endl;
    std::cout << "Client Max Body Size: " << clientMaxBodySize << std::endl;
    for (std::map<int, std::string>::const_iterator it = errorPages.begin(); it != errorPages.end(); ++it) {
        std::cout << "Error Page: " << it->first << " -> " << it->second << std::endl;
    }
    std::cout << std::endl;
}

size_t ServerConfig::getListen() {
    return (listen);
}