/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Library.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 16:19:03 by haejeong          #+#    #+#             */
/*   Updated: 2024/08/09 20:08:10 by sangyhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBRARY_HPP
# define LIBRARY_HPP

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <fcntl.h>
#include <algorithm>
#include <map>
#include <set>
#include <vector>
#include <fstream>
#include <sstream>
#include <cctype>
#include <sys/stat.h>
#include <ctime>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <dirent.h>

#define BUFFER_SIZE 10000
#define MAX_BODY_SIZE 30000000
#define READ_END "\r\n\r\n"

#define ERR400 "Bad Request"
#define ERR403 "Forbidden"
#define ERR404 "Not Found"
#define ERR405 "Method Not Allowed"
#define ERR408 "Request Timeout"
#define ERR411 "Length Required"
#define ERR413 "Payload Too Large"
#define ERR415 "Unsupported Media Type"
#define ERR500 "Internal Server Error"
#define ERR501 "Not Implemented"

enum METHOD {
    NONE = 1,
    GET = 2,
    POST = 3,
    DELETE = 4,
    PUT = 5,
    CONNECT = 6,
    OPTION = 7,
    HEAD = 8,
};

enum REQUEST_STATE {
    REQUEST_START = 1,
    HEADER_END = 2,
    CONTENT_END = 3,
};

struct HttpRequest {
    METHOD      method;
    std::string target;
    std::string url;
    std::string httpVersion;
    std::string host;
    std::string userAgent;
    std::string accept;
    std::string contentType;
    std::string contentLength;
    std::map<std::string, std::string> headers;
    std::string query;
    size_t bodyStart;
    size_t bodyEnd;
    bool error;
    
    int fd;
    size_t fileCount;
    bool                isCgi;
    bool                chunked;

    std::map<int, std::string>      errorPages;
    std::pair<int, std::string>     redirection;

    HttpRequest()
        : method(NONE),
        url("/"), 
        httpVersion(""), 
        host(""), 
        userAgent(""), 
        accept(""),
        // contentType("text/html"),
        contentLength(""),
        query("") ,
        error(false),
        isCgi(false),
        chunked(false) {}
};

void	setNonblock(int fd);

std::vector<std::string> ft_split(std::string & str, char c);

int isDirectory(std::string & path);

bool isHexDigit(char c);

int hexToDecimal(const std::string & hex);

bool endWith(const std::string& str, const std::string& suffix);

bool isCGI(const std::string & str);

void pushEvent(int fd, int filter, int flag, std::vector <struct kevent> &changeList);

bool isCgiFile(const std::string& filename);

char **arrFree(char **arr);

std::string getContentType(std::string url);

class RuntimeException : public std::runtime_error {
public:
    RuntimeException(const std::string& message) 
        : std::runtime_error(message) {}
};

std::string ft_toString(int num);

std::string readFileToString(const std::string& filePath);

std::string makeRealErrorPath(const std::string & code, const std::string & errorPath);

std::string returnErrorMsg(int code);

#endif