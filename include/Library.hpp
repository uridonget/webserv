/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Library.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 16:19:03 by haejeong          #+#    #+#             */
/*   Updated: 2024/07/03 18:15:13 by sangyhan         ###   ########.fr       */
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

#define BUFFER_SIZE 1000
#define MAX_BODY_SIZE 30000000
#define READ_END "\r\n\r\n"

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
    METHOD method; // 일단 string으로 저장할 것. 나중에 enum METHOD로 바꾸는 일이 있을수도?
    std::string url;
    std::string httpVersion;
    std::string host;
    std::string userAgent;
    std::string accept;
    std::string contentLength;
    std::map<std::string, std::string> headers;
    std::vector<char> body;

    HttpRequest()
        : method(NONE), 
        url(""), 
        httpVersion(""), 
        host(""), 
        userAgent(""), 
        accept(""),
        contentLength("") {}
};

void	setNonblock(int fd);
// std::string	readFile(const char* filename);

class RuntimeException : public std::runtime_error {
public:
    RuntimeException(const std::string& message) 
        : std::runtime_error(message) {}
};

#endif