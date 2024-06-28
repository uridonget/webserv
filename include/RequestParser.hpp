/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 21:10:04 by sangyhan          #+#    #+#             */
/*   Updated: 2024/06/28 16:39:57 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include "Library.hpp"

class RequestParser
{
private:
    static int trie[10][26];
    size_t kmp(std::vector<char> &buf, std::string &target, size_t start);
    size_t findEnd(std::vector<char> &buf, char *append, size_t size);
    HttpRequest request;

public:
    static const size_t npos = 18446744073709551615UL;
    RequestParser() {};
    ~RequestParser() {};
    size_t checkEnd(std::vector<char> &buf, char *append, size_t size, size_t & endHeader);
    struct HttpRequest requestParsing(std::vector<char> fullRequest, size_t endIndex, size_t & endHeader);
};

#endif