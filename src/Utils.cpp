/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 16:46:15 by heolee            #+#    #+#             */
/*   Updated: 2024/07/12 12:42:41 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Library.hpp"

void setNonblock(int fd) {
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
		throw RuntimeException("fcntl");
	}
}

std::string readFile(const char* filename) {
	std::ifstream file(filename);
	if (!file) {
		return "";
	}
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	return content;
}

std::vector<std::string> ft_split(std::string str, char c) {
    std::vector<std::string> words;
    std::stringstream sstream(str);
    std::string word;
    while (getline(sstream, word, c)) {
        if (word == "..") {
            if (!words.empty())
                words.pop_back();
        }
        else if (word == "." || word.empty())
            continue;
        else
            words.push_back(word);
    }
    return words;
}

/*
return 1 : 경로에 파일이 없음
return 2 : 경로에 파일이 있고 그 파일이 디렉토리가 아님
return 3 : 경로에 파일이 있고 그 파일이 디렉토리
*/
int isDirectory(std::string & path) {
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0) {
        return 1; // 경로에 파일이 없음
    }
    if (!S_ISDIR(statbuf.st_mode)) {
        return 2; // 경로에 파일이 있고 그 파일이 디렉토리가 아님
    } else {
        return 3; // 경로에 파일이 있고 그 파일이 디렉토리임
    }
}

bool isHexDigit(char c) {
    return std::isxdigit(static_cast<unsigned char>(c));
}

int hexToDecimal(const std::string & hex) {
    int decimal;
    std::stringstream ss;
    ss << std::hex << hex;
    ss >> decimal;
    return decimal;
}
