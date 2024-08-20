/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MimeParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/06 13:06:19 by sangyhan          #+#    #+#             */
/*   Updated: 2024/07/29 13:55:01 by sangyhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MIMEPARSER_HPP
# define MIMEPARSER_HPP

#include "Library.hpp"
#include "RequestParser.hpp"
#include "File.hpp"

class MimeParser
{
private:
    char currentChar;
    std::vector<char> &buf;
    size_t bufIndex;
    size_t end;
    std::string boundary;
    std::vector<Buffer *> request;
    static const char NULL_CHAR = '\0';
    static const char CR = '\r';
    static const char LF = '\n';
    static const size_t npos = 18446744073709551615UL;

    size_t kmp(std::string &target);
    void nextChar();
    void consumeOWS();
    int consumeCRLF();
    int consumeMimeEnd();
    int consumeBoundary();
    void deleteFileAll(std::vector<Buffer *> &request);
    int fieldLine(std::vector<std::pair<std::string, std::string> > &header);
    std::string parseFieldName();
    std::string parseFieldValue();
    int consumeUntilBoundary(bool fileopen);
    int expect(char expected);

public:
    MimeParser(std::string boder, std::vector<char> &buf, size_t start, size_t end);
    std::vector<Buffer *> parse(std::string root);
};

#endif