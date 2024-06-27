/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 21:10:04 by sangyhan          #+#    #+#             */
/*   Updated: 2024/06/27 17:03:30 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include <vector>
#include <string>

enum METHOD
{
    NONE = 1,
    GET = 2,
    POST = 3,
    DELETE = 4,
    PUT = 5,
    CONNECT = 6,
    OPTION = 7,
};

enum REQUEST_STATE
{
    REQUEST_START = 1,
    HEADER_END = 2,
    CONTENT_END = 3,
};

class RequestParser
{
private:
    static int trie[10][26];
    size_t kmp(std::vector<char> &buf, std::string &target, size_t start);
    size_t findEnd(std::vector<char> &buf, char *append, size_t size);
    enum METHOD checkMethod();

public:
    static const size_t npos = 18446744073709551615UL;
    RequestParser() {};
    ~RequestParser() {};
    size_t checkEnd(std::vector<char> &buf, char *append, size_t size);
};

#endif