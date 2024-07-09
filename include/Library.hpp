/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Library.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 16:19:03 by haejeong          #+#    #+#             */
/*   Updated: 2024/07/09 19:28:05 by haejeong         ###   ########.fr       */
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

#define BUFFER_SIZE 800
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

enum METHOD {
    NONE = 1,
    GET = 2,
    POST = 3,
    DELETE = 4,
    PUT = 5,
    CONNECT = 6,
    OPTION = 7,
};

enum REQUEST_STATE {
    REQUEST_START = 1,
    HEADER_END = 2,
    CONTENT_END = 3,
};

struct HttpRequest {
    METHOD      method;
    std::string url;
    std::string httpVersion;
    std::string host;
    std::string userAgent;
    std::string accept;
    std::string contentType;
    std::string contentLength;
    std::map<std::string, std::string> headers;
    std::string query;
    // std::map<std::string, std::string> query;
    int fd;
    std::vector<char> body;

    HttpRequest()
        : method(NONE),
        url("/"), 
        httpVersion(""), 
        host(""), 
        userAgent(""), 
        accept(""),
        contentLength(""),
        query("") {}
};

void	setNonblock(int fd);

std::vector<std::string> ft_split(std::string str, char c);

int isDirectory(std::string & path);

class RuntimeException : public std::runtime_error {
public:
    RuntimeException(const std::string& message) 
        : std::runtime_error(message) {}
};

#endif