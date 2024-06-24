/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParsing.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 15:18:15 by haejeong          #+#    #+#             */
/*   Updated: 2024/06/24 16:30:34 by haejeong         ###   ########.fr       */
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
	for (int i=0; i < line.length(); i++) {
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

void ConfigParsing::splitServer(std::string & fileInput) {
	
}

void ConfigParsing::checkComment(std::string & fileInput) {
	std::cout << "check : \n" << fileInput << std::endl;
}