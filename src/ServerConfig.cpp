/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 14:31:23 by haejeong          #+#    #+#             */
/*   Updated: 2024/07/22 13:40:36 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ServerConfig.hpp"

ServerConfig::ServerConfig()
	: listen(0),
	serverName(""),
	root("/"),
	index("index.html"),
	autoindex(false),
	clientMaxBodySize(INT_MAX) {
}

ServerConfig::~ServerConfig() {}

static bool isNumber(std::string num) {
	for (size_t i = 0; i < num.length(); i++) {
		if (!isdigit(num[i])) {
			std::cout << num[i] << std::endl;
			return false;
		}
	}
	return true;
}

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
	bool inLocationBlock = false;
	bool inExtensionBlock = false;
	Location currentLocation;
	Location currentExtension;

	while (std::getline(iss, line)) {
		tokens.clear();
		std::istringstream lineStream(line);
		while (lineStream >> temp) {
			tokens.push_back(temp);
		}
		if (tokens[0] == "listen") {
			if (listen || tokens.size() != 2 || !checkSemiColon(tokens) || !isNumber(tokens[1]))
				throw RuntimeException("invalid listen in configuration file");
			listen = std::atoi(tokens[1].c_str());
		}
		else if (tokens[0] == "server_name") {
			if (tokens.size() != 2 || !checkSemiColon(tokens))
				throw RuntimeException("invalid server_name in configuration file");
			serverName = tokens[1];
		}
		
		else if (tokens[0] == "location") {
			if (tokens.size() != 3 || tokens[tokens.size() - 1] != "{")
				throw RuntimeException("invalid location in configuration file");
			currentLocation.setPath(tokens[1]);
			inLocationBlock = true;
		}
		else if (tokens[0] == "extension") {
			if (tokens.size() != 3 || tokens[tokens.size() - 1] != "{")
				throw RuntimeException("invalid extension in configuration file");
			currentExtension.setPath(tokens[1]);
			inExtensionBlock = true;
		}
		else if (tokens[0] == "root") {
			if (tokens.size() < 2 || !checkSemiColon(tokens))
				throw RuntimeException("invalid root in configuration file");
			if (inLocationBlock) {
				currentLocation.setRoot(tokens[1]);
			} else if (inExtensionBlock) {
				currentExtension.setRoot(tokens[1]);
			} else {
				root = tokens[1];
			}
		}
		else if (tokens[0] == "index") {
			if (tokens.size() != 2 || !checkSemiColon(tokens))
				throw RuntimeException("invalid index in configuration file");
			if (inLocationBlock) {
				currentLocation.setIndex(tokens[1]);
			} else if (inExtensionBlock) {
				currentExtension.setIndex(tokens[1]);	
			} else {
				index = tokens[1];
			}
		}
		else if (tokens[0] == "autoindex") {
			if (tokens.size() != 2 || !checkSemiColon(tokens))
				throw RuntimeException("invalid autoindex in configuration file");
			if (inLocationBlock) {
				if (tokens[1] == "on") {
					currentLocation.setAutoindex(true);
				} else if (tokens[1] == "off") {
					currentLocation.setAutoindex(false);
				} else {
					throw RuntimeException("invalid autoindex in configuration file");
				}
			} else {
				if (tokens[1] == "on") {
					autoindex = true;
				} else if (tokens[1] == "off") {
					autoindex = false;
				} else {
					throw RuntimeException("invalid autoindex in configuration file");
				}
			}
		}
		else if (tokens[0] == "error_page") {
			if (tokens.size() < 2 || !checkSemiColon(tokens))
				throw RuntimeException("invalid error_page in configuration file");
			std::string errorPath = tokens[tokens.size() - 1];
			if (inLocationBlock) {
				std::map<int, std::string> & errorPagesLocation = currentLocation.getErrorPages();
				if (errorPagesLocation.size())
					errorPagesLocation.clear();
				for (size_t i = 1; i < tokens.size() - 1; i++) {
					if (!isNumber(tokens[i]))
						throw RuntimeException("invalid error_page in configuration file");
					errorPagesLocation[std::atoi(tokens[i].c_str())] = makeRealErrorPath(tokens[i], errorPath);
				}
			} else {
				if (errorPages.size())
					errorPages.clear();
				for (size_t i = 1; i < tokens.size() - 1; i++) {
					if (!isNumber(tokens[i]))
						throw RuntimeException("invalid error_page in configuration file");
					errorPages[std::atoi(tokens[i].c_str())] = makeRealErrorPath(tokens[i], errorPath);
				}
			}	
		} 
		else if (tokens[0] == "return") {
			if (tokens.size() != 3 || !checkSemiColon(tokens) || !isNumber(tokens[1]))
				throw RuntimeException("invalid return in configuration file");
			if (inLocationBlock) {
				currentLocation.setRedirection(std::atoi(tokens[1].c_str()), tokens[2]); 
			} else {
				redirection.first = std::atoi(tokens[1].c_str());
				redirection.second = tokens[2];
			}
		}
		else if (tokens[0] == "allowed_methods") {
            if (!checkSemiColon(tokens))
                throw RuntimeException("invalid return in configuration file");
            if (inLocationBlock) {
                std::set<METHOD> methods;
                for (size_t i = 1; i < tokens.size(); i++) {
                    if (tokens[i] == "GET") {
                        methods.insert(GET);
                    } else if (tokens[i] == "POST") {
                        methods.insert(POST);
                    } else if (tokens[i] == "DELETE") {
                        methods.insert(DELETE);
                    }
                }
                currentLocation.setAllowedMethods(methods);
            } else {
                allowedMethods.clear();
                for (size_t i = 1; i < tokens.size(); i++) {
                    if (tokens[i] == "GET") {
                        allowedMethods.insert(GET);
                    } else if (tokens[i] == "POST") {
                        allowedMethods.insert(POST);
                    } else if (tokens[i] == "DELETE") {
                        allowedMethods.insert(DELETE);
                    }
                }
            }
        }
		else if (tokens[0] == "client_max_body_size") {
			if (tokens.size() != 2 || !checkSemiColon(tokens) || !isNumber(tokens[1]))
				throw RuntimeException("invalid client_max_body_size in configuration file");
			if (inLocationBlock) {
				currentLocation.setClientMaxBodySize(std::stoul(tokens[1]));
			} else {
				clientMaxBodySize = std::stoul(tokens[1]);
			}
		} 
        else if (tokens[0] == "}") {
            if (tokens.size() != 1)
                throw RuntimeException("invalid '}' in configuration file");
            if (inLocationBlock) {
				for (std::map<int, std::string>::iterator it = currentLocation.getErrorPages().begin(); it != currentLocation.getErrorPages().end(); ++it) {
					std::string realErrorPagePath = currentLocation.getRoot() + (*it).second;
					(*it).second = readFileToString(realErrorPagePath);
				}
				
                locationList.push_back(currentLocation);
                currentLocation = Location();
                inLocationBlock = false;
            } else if (inExtensionBlock) {
                extensionList.push_back(currentExtension);
                currentExtension = Location();
                inExtensionBlock = false;
            } else {
                throw RuntimeException("invalid '}' in configuration file");
			}
        }
    }
    if (inLocationBlock || inExtensionBlock) {
        throw RuntimeException("invalid location or extension block in configuration file 1");
    }
	for (std::map<int, std::string>::iterator it = errorPages.begin(); it != errorPages.end(); ++it) {
		std::string realErrorPagePath = root + (*it).second;
		(*it).second = readFileToString(realErrorPagePath);
	}
}

void ServerConfig::printConfig() {
    std::cout << "Listen: " << listen << std::endl;
    std::cout << "Server Name: " << serverName << std::endl;
    std::cout << "Root: " << root << std::endl;
    std::cout << "Index: " << index << std::endl;
	if (autoindex) {
		std::cout << "AutoIndex: on" << std::endl;
	} else {
		std::cout << "AutoIndex: off" << std::endl;
	}
    std::cout << "Client Max Body Size: " << clientMaxBodySize << std::endl;
	for (std::map<int, std::string>::const_iterator it = errorPages.begin(); it != errorPages.end(); ++it) {
        std::cout << "Error Page: " << it->first << " -> " << it->second << std::endl;
    }
    if (allowedMethods.size()) {
        std::cout << "Allowed Methods : ";
        for (std::set<METHOD>::iterator it = allowedMethods.begin(); it != allowedMethods.end(); it++) {
            if (*it == GET) {
                std::cout << "GET ";
            } else if (*it == POST) {
                std::cout << "POST ";
            } else if (*it == DELETE) {
                std::cout << "DELETE ";
            }
        }
        std::cout << std::endl;
    }
    for (std::vector<Location>::iterator it = locationList.begin(); it != locationList.end(); ++it) {
        (*it).showLocation();
    }
	for (std::vector<Location>::iterator it = extensionList.begin(); it != extensionList.end(); ++it) {
		(*it).showLocation();
	}
    std::cout << std::endl;
}

size_t ServerConfig::getListen() const {
    return listen;
}

std::string ServerConfig::getServerName() const {
    return serverName;
}

std::string ServerConfig::getRoot() const {
    return root;
}

std::string ServerConfig::getIndex() const {
    return index;
}

bool ServerConfig::getAutoindex() const {
	return autoindex;
}

std::map<int, std::string> & ServerConfig::getErrorPages() {
    return errorPages;
}

std::pair<int, std::string> ServerConfig::getRedirection() const {
    return redirection;
}

std::vector<Location> ServerConfig::getLocationList() const {
    return locationList;
}

std::vector<Location> ServerConfig::getExtensionList() const {
    return extensionList;
}

std::set<METHOD> ServerConfig::getAllowedMethods() const {
    return allowedMethods;
}

unsigned long ServerConfig::getClientMaxBodySize() const {
    return clientMaxBodySize;
}
