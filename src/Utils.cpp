/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 16:46:15 by heolee            #+#    #+#             */
/*   Updated: 2024/07/19 15:36:51 by haejeong         ###   ########.fr       */
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

std::vector<std::string> ft_split(std::string & str, char c) {
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

bool endWith(const std::string& str, const std::string& suffix) {
	if (str.length() < suffix.length()) {
		return false;
	}
	return (str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0);
}

bool isCGI(const std::string& str) {
	std::string suffix1 = ".py";
	std::string suffix2 = ".php";
	std::string suffix3 = "cgi_tester";

	if (endWith(str, suffix1) || endWith(str, suffix2) || endWith(str, suffix3)) {
		return true;
	}
	return false;
}

void pushEvent(int fd, int filter, int flag, std::vector <struct kevent> &changeList)
{
    struct kevent change;
    EV_SET(&change, fd, filter, flag, 0, 0, NULL);
    changeList.push_back(change);
}

bool isCgiFile(const std::string& filename) {
    const std::string extension = ".cgi";
    if (filename.length() < extension.length()) {
        return false;
    }
    return filename.compare(filename.length() - extension.length(), extension.length(), extension) == 0;
}

char **arrFree(char **arr) {
	int	i;

	i = 0;
	while (arr[i]) {
		delete arr[i];
		i++;
	}
	delete arr;
	return (NULL);
}

std::string getContentType(std::string url)
{
    size_t pos = url.find_last_of(".", std::string::npos);
    std::string res;
    if (pos == std::string::npos)
    {
        res = "text/plain";
    }
    else
    {
        std::string filetype = url.substr(pos + 1, std::string::npos);
        if (filetype == "js" || filetype == "css" || filetype == "html" || filetype == "javascript" || filetype == "csv"
            || filetype == "xml")
        {
            res += "text/";
            res += filetype;
        }
        else if (filetype == "jpeg" || filetype == "jpg" || filetype == "png" || filetype == "gif" || filetype == "tiff")
        {
            res += "image/";
            res += filetype;
        }
        else if (filetype == "mpeg")
        {
            res += "audio/";
            res += filetype;
        }
        else if (filetype == "mpeg" || filetype == "mp4" || filetype == "webm" || filetype == "mov")
        {
            res += "vedio/";
            res += filetype;
        }
    }
    return res;
}

std::string ft_toString(int num) {
    std::stringstream ss;
    ss << num;
    return ss.str();
}

std::string readFileToString(const std::string& filePath) {
    std::ifstream file(filePath.c_str());
    if (!file.is_open()) {
        std::cout << "FILE PATH: " << filePath.c_str() << std::endl;
        throw RuntimeException("invalid Error Page file");
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    return buffer.str();
}

std::string makeRealErrorPath(const std::string & code, const std::string & errorPath) {
    std::string realErrorPath;
    for (size_t i = 2; i < errorPath.length() ;i++) {
        if (errorPath[i - 2] == '4' || errorPath[i - 2] == '5') {
            if (errorPath[i] == 'x') {
                realErrorPath = errorPath;
                realErrorPath[i - 2] = code[0];
                realErrorPath[i - 1] = code[1];
                realErrorPath[i] = code[2];
                break ;
            }
        }
    }
    if (realErrorPath.length() == 0) {
        throw RuntimeException("Invalid error page!");
    }
    return realErrorPath;
}

std::string returnErrorMsg(int code) {
    std::string message;

    if (code / 100 == 3) {
        if (code == 301) {
            message = "Moved Permanently";
        } else if (code == 303) {
            message = "See Other";
        }
    } else if (code / 100 == 4) {
        if (code == 400) {
            message = ERR400;
        } else if (code == 403) {
            message = ERR403;
        } else if (code == 404) {
            message = ERR404;
        } else if (code == 405) {
            message = ERR405;
        } else if (code == 408) {
            message = ERR408;
        } else if (code == 411) {
            message = ERR411;
        } else if (code == 413) {
            message = ERR413;
        } else if (code == 415) {
            message = ERR415;
        }
    } else {
        if (code == 500) {
            message = ERR500;
        } else if (code == 501) {
            message = ERR501;
        }
    }
    return message;
}