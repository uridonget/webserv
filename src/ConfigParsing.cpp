/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParsing.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 15:18:15 by haejeong          #+#    #+#             */
/*   Updated: 2024/07/18 13:45:23 by sangyhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ConfigParsing.hpp"

ConfigParsing::ConfigParsing() {};

ConfigParsing::~ConfigParsing() {};

void ConfigParsing::removeComment(std::string & fileInput) {
	std::string::iterator start, end;

	while ((start = std::find(fileInput.begin(), fileInput.end(), '#')) != fileInput.end()) {
        end = std::find(start, fileInput.end(), '\n');
        fileInput.erase(start, end);
    }
}

static bool isEmptyLine(std::string & line) {
	for (unsigned long i = 0; i < line.length(); i++) {
		if (!(std::isspace(line[i]))) {
			return false;
		}
	}
	return true;
}

static void removeEmptySpace(std::string & line) {
	for (std::string::iterator it = line.begin(); *it != '\n'; ++it) {
		if (!(std::isspace(*it))) {
			line.erase(line.begin(), it);
			return ;
		}
	}
}

void ConfigParsing::removeEmptyLine(std::string & fileInput) {
	std::istringstream iss(fileInput);
    std::ostringstream oss;
    std::string line;

    while (std::getline(iss, line)) {
		if (!(isEmptyLine(line))) {
			removeEmptySpace(line);
			oss << line << '\n';
		}
    }
	fileInput = oss.str();
}

void ConfigParsing::insertServerConfig(int start, std::string &fileInput) {
	std::string serverConfig;
	
	int cnt = 0;
	int len = static_cast<int>(fileInput.length());
	for (int temp = start + 1; temp < len; temp++) {
		if (fileInput[temp] == '{') {
			cnt++;
		}
		else if (fileInput[temp] == '}') {
			if (cnt == 0) {
				serverConfig = fileInput.substr(start, temp - start - 1);
				configStrings.push_back(serverConfig);
				fileInput = fileInput.substr(temp + 1, len - temp - 1);
				return ;
			}
			cnt--;
		}
	}
	throw RuntimeException("Invalid configuration file");
}

void ConfigParsing::splitServer(std::string & fileInput) {
	while (1) {
		unsigned long i = 0;
		while (fileInput[i] && isspace(fileInput[i]))
			i++;
		if (i == fileInput.length())
			return ;
		else if (fileInput.compare(i, 6, "server") != 0) {
			throw RuntimeException("Invalid configuration file [server]");
		}
		i += 6;
		while (fileInput[i] && isspace(fileInput[i]))
			i++;
		if (fileInput[i] == '{') {
			while (fileInput[i + 1] && isspace(fileInput[i + 1]))
				i++;
			insertServerConfig(i + 1, fileInput);
		} else {
			throw RuntimeException("Invalid configuration file [server]");
		}
	}
}

void ConfigParsing::setServerConfig() {
	for (std::vector<std::string>::iterator it = configStrings.begin(); it != configStrings.end(); it++) {
		ServerConfig config;
		config.parseConfig(*it);
		config.printConfig();
		ServerConfigs.push_back(config);
	}
}

std::vector<ServerConfig> ConfigParsing::getServerConfigs() {
    return (this->ServerConfigs);
}
