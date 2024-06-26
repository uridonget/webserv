/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 14:31:23 by haejeong          #+#    #+#             */
/*   Updated: 2024/06/26 14:15:00 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ServerConfig.hpp"

ServerConfig::ServerConfig()
    : listen(0),
    serverName(""),
    root("/"),
    clientMaxBodySize(0),
    index("index.html"),
    autoindex(false) {
    // std::cout << "ServerConfig default constructor called" << std::endl;
}

ServerConfig::~ServerConfig() {
	// std::cout << "ServerConfig destructor called" << std::endl;
}

static bool isNumber(std::string num) {
    for (int i=0; i < num.length(); i++) {
        if (!isdigit(num[i])) {
            std::cout << num[i] << std::endl;
            return false;
        }
    }
    return true;
}

// void ServerConfig::parseConfig(const std::string& configString) {
//     std::istringstream iss(configString);
//     std::string line;

//     Location currentLocation;
//     bool inLocationBlock = false;

//     while (std::getline(iss, line)) {
//         std::istringstream lineStream(line);
//         std::string key;
//         lineStream >> key;
//         if (key == "listen") {
//             if (listen)
//                 throw RuntimeException("multiple listen in configuration file");
//             std::string port;
//             lineStream >> port;
//             if (!isNumber(port)) {
//                 std::cout << port << std::endl;
//                 throw RuntimeException("invalid listen in configuration file");
//             }
//             listen = std::stoi(port);
//         }
//         else if (key == "server_name") {
//             lineStream >> serverName;
//         } 
//         else if (key == "root") {
//             lineStream >> root;
//         } 
//         else if (key == "client_max_body_size") {
//             lineStream >> clientMaxBodySize;
//         } 
//         else if (key == "index") {
//             lineStream >> index;
//         } 
//         else if (key == "error_page") {
//             int errorCode;
//             std::string errorPath;
//             lineStream >> errorCode >> errorPath;
//             errorPages[errorCode] = errorPath;
//         } 
//         else if (key == "return") {
//             int code;
//             std::string path;
//             lineStream >> code >> path;
//             redirection = path;
//         } 
//         else if (key == "location") {
//             if (inLocationBlock) {
//                 locationList.push_back(currentLocation);
//                 currentLocation = Location();
//             }
//             std::string locationPath;
//             lineStream >> locationPath;
//             currentLocation.setPath(locationPath);
//             inLocationBlock = true;
//         } 
//         else if (key == "autoindex") {
//             std::string value;
//             lineStream >> value;
//             currentLocation.setAutoIndex(value == "on");
//         } 
//         else if (key == "index" && inLocationBlock) {
//             std::string idx;
//             lineStream >> idx;
//             currentLocation.setIndex(idx);
//         } 
//         else if (key == "root" && inLocationBlock) {
//             std::string rt;
//             lineStream >> rt;
//             currentLocation.setRoot(rt);
//         } 
//         else if (key == "}") {
//             if (inLocationBlock) {
//                 locationList.push_back(currentLocation);
//                 currentLocation = Location();
//                 inLocationBlock = false;
//             }
//         }
//     }
//     if (inLocationBlock) {
//         locationList.push_back(currentLocation);
//     }
// }

static bool checkSemiColon(std::vector<std::string> & tokens) {
    std::string & last = tokens[tokens.size() - 1];
    if (last[last.size() - 1] == ';') {
        last.erase(last.size() - 1);
        return true;
    }
    return false;
}

void ServerConfig::parseConfig(const std::string& configString) {
    std::istringstream iss(configString);
    std::string line;
    std::vector<std::string> tokens;
    std::string temp;
    int cnt;
    bool inLocationBlock = false;
    Location currentLocation;

    while (std::getline(iss, line)) {
        tokens.clear();
        std::istringstream lineStream(line);
        while (lineStream >> temp) {
            tokens.push_back(temp);
        }
        if (tokens[0] == "listen") {
            if (listen || tokens.size() != 2 || !checkSemiColon(tokens) || !isNumber(tokens[1]))
                throw RuntimeException("invalid listen in configuration file");
            listen = std::stoi(tokens[1]);
        }
        else if (tokens[0] == "server_name") {
            if (tokens.size() != 2 || !checkSemiColon(tokens))
                throw RuntimeException("invalid server_name in configuration file");
            serverName = tokens[1];
        }
        else if (tokens[0] == "client_max_body_size") {
            if (tokens.size() != 2 || !checkSemiColon(tokens) || !isNumber(tokens[1]))
                throw RuntimeException("invalid client_max_body_size in configuration file");
            clientMaxBodySize = std::stoul(tokens[1]);
        }
        else if (tokens[0] == "error_page") {
            if (errorPages.size())
                errorPages.clear();
            if (tokens.size() < 2 || !checkSemiColon(tokens))
                throw RuntimeException("invalid error_page in configuration file");
            std::string errorPath = tokens[tokens.size() - 1];
            for (int i=1; i < tokens.size() - 1; i++) {
                if (!isNumber(tokens[i]))
                    throw RuntimeException("invalid error_page in configuration file");
                errorPages[std::stoi(tokens[i])] = errorPath;
            }
        }
        else if (tokens[0] == "location") {
            if (tokens.size() != 3)
                throw RuntimeException("invalid location in configuration file");
            if (tokens[tokens.size() - 1] != "{")
                throw RuntimeException("invalid location in configuration file");
            currentLocation.setPath(tokens[1]);
            inLocationBlock = true;
        }
        else if (tokens[0] == "root") {
            if (tokens.size() != 2 || !checkSemiColon(tokens))
                throw RuntimeException("invalid root in configuration file");
            if (inLocationBlock) {
                currentLocation.setRoot(tokens[1]);
            } else
                root = tokens[1];
        }
        else if (tokens[0] == "index") {
            if (tokens.size() != 2 || !checkSemiColon(tokens))
                throw RuntimeException("invalid index in configuration file");
            if (inLocationBlock) {
                currentLocation.setIndex(tokens[1]);
            } else
                index = tokens[1];
        }
        else if (tokens[0] == "autoindex") {
            if (tokens.size() != 2 || !checkSemiColon(tokens))
                throw RuntimeException("invalid autoindex in configuration file");
            if (inLocationBlock) { // location autoindex
                currentLocation.setAutoIndex(tokens[1] == "on");
            } else { // server autoindex
                autoindex = (tokens[1] == "on");
            }
        }
        else if (tokens[0] == "return") {
            if (tokens.size() != 3 || !checkSemiColon(tokens) || !isNumber(tokens[1]))
                throw RuntimeException("invalid return in configuration file");
            if (inLocationBlock) {
                currentLocation.setRedirection(std::stoi(tokens[1]), tokens[2]); 
            } else {
                redirection.first = std::stoi(tokens[1]);
                redirection.second = tokens[2];    
            }
        } 
        else if (tokens[0] == "}") {
            if (tokens.size() != 1)
                throw RuntimeException("invalid '}' in configuration file");
            if (inLocationBlock) { // location block 이면 location 추가하고 닫기
                locationList.push_back(currentLocation);
                currentLocation = Location();
                inLocationBlock = false;
            } else
                throw RuntimeException("invalid '}' in configuration file");
        }
    }
    if (inLocationBlock) {
        throw RuntimeException("invalid location block in configuration file 1");
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
    for (std::vector<Location>::iterator it = locationList.begin(); it != locationList.end(); it++) {
        (*it).showLocation();
    }
    std::cout << std::endl;
}

size_t ServerConfig::getListen() {
    return (listen);
}