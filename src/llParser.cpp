/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   llParser.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/02 14:08:20 by sangyhan          #+#    #+#             */
/*   Updated: 2024/07/31 13:40:22 by sangyhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/llParser.hpp"


class ParseException : public std::runtime_error
{
public:
    ParseException(const std::string &msg) : std::runtime_error(msg) {}
};

llParser::llParser(const std::vector<char> &buffer, size_t endHeader) : buf(buffer)
{
    currentChar = NULL_CHAR;
    end = endHeader + 4;
    bufIndex = 0;
    if (bufIndex < buf.size() && bufIndex < end)
        currentChar = buf[bufIndex];
    else
        currentChar = NULL_CHAR;
}

static void urlParsing(HttpRequest & request, std::string rawUrl) {
    std::string tempUrl;
    for (size_t i = 0; i < rawUrl.size(); i++)
    {
        if (rawUrl[i] == '%')
        {
            if (i + 2 < rawUrl.size())
            {
                char decode = (std::tolower(rawUrl[i + 1]) - 'a') * 16 + std::tolower(rawUrl[i + 2] - 'a');
                tempUrl += decode;
                i += 2;
            }
            else
            {
                tempUrl += rawUrl[i];
            }
        }
        else
            tempUrl += rawUrl[i];
    }
    rawUrl = tempUrl;
    tempUrl = "";
    std::string::size_type pos = rawUrl.find('?');
    if (pos != std::string::npos) {
        tempUrl = rawUrl.substr(0, pos);
        request.query = rawUrl.substr(pos + 1);
    } else {
        tempUrl = rawUrl;
    }
    std::string res = "/";
    std::vector<std::string> splitUrl = ft_split(tempUrl, '/');
    if (!splitUrl.empty()) {
        std::vector<std::string>::iterator it = splitUrl.begin();
        for (; it != splitUrl.end() - 1; ++it) {
            res += *it;
            res += '/';
        }
        res += *it;
    }
    if (tempUrl.size() != 1 && tempUrl[tempUrl.size() - 1] == '/') {
        res += '/';
    }
    request.url = res;
}

HttpRequest llParser::parse()
{
	HttpRequest request;
	std::string method = parseToken();
	if (method == "GET") {
		request.method = GET;
	} else if (method == "POST") {
		request.method = POST;
	} else if (method == "DELETE") {
		request.method = DELETE;
	} else if (method == "HEAD") {
        request.method = HEAD;
    }
	consumeSP();
    urlParsing(request, parseToken());
	consumeSP();
	request.httpVersion = parseToken();
	consumeCRLF();
	while (currentChar != NULL_CHAR && currentChar != CR)
	{
		std::pair<std::string, std::string> header = fieldline();
		request.headers[header.first] = header.second;
	}
    consumeCRLF();
    expect('\0');
    if (request.headers.find("Host") != request.headers.end())
    {
        request.host = request.headers["Host"];
    }
    if (request.headers.find("User-Agent") != request.headers.end())
    {
        request.userAgent = request.headers["User-Agent"];
    }
    if (request.headers.find("Accept") != request.headers.end())
    {
        request.accept = request.headers["Accept"];
    }
    if (request.headers.find("Content-Type") != request.headers.end())
    {
        request.contentType = request.headers["Content-Type"];
    }
    if (request.headers.find("Content-Length") != request.headers.end())
    {
        request.contentLength = request.headers["Content-Length"];
    }
    if (request.headers.find("Transfer-Encoding: chunked") != request.headers.end())
    {
        request.chunked = true;
    }
    return request;
}

void llParser::nextChar()
{
    if (bufIndex + 1 < buf.size() && bufIndex + 1 < end)
    {
        bufIndex++;
        currentChar = buf[bufIndex];
    }
    else
    {
        currentChar = NULL_CHAR;
    }
}

void llParser::consumeSP()
{
    if (currentChar == SP)
    {
        nextChar();
    }
    else
    {
        throw ParseException("Expected SP");
    }
}

void llParser::consumeCRLF()
{
    if (currentChar == CR)
    {
        nextChar();
        if (currentChar == LF)
        {
            nextChar();
        }
        else
        {
            throw ParseException("Expected LF after CR");
        }
    }
    else
    {
        throw ParseException("Expected CRLF");
    }
}

std::string llParser::parseToken()
{
	std::string token;
	while (currentChar != CR && currentChar != LF && currentChar != NULL_CHAR && currentChar != SP && currentChar != '\t')
	{
		token += currentChar;
		nextChar();
	}
	if (token.size() == 0)
	{
		throw ParseException("Expected token");
	}
	return token;
}

std::pair<std::string, std::string> llParser::fieldline()
{
    std::string name = parseFieldName();
    expect(':');
    nextChar();
    consumeOWS();
    std::string value = parseFieldValue();
    consumeCRLF();
    return (std::make_pair(name, value));
}

std::string llParser::parseFieldName()
{
    std::string name;
    while (currentChar != CR && currentChar != LF && currentChar != NULL_CHAR && currentChar != SP && currentChar != '\t' && currentChar != ':')
    {
        name += currentChar;
        nextChar();
    }
    return name;
}

std::string llParser::parseFieldValue()
{
    std::string value;
    while (currentChar != CR && currentChar != LF && currentChar != NULL_CHAR)
    {
        value += currentChar;
        nextChar();
    }
    return value;
}

void llParser::consumeOWS()
{
    while (currentChar == ' ')
    {
        nextChar();
    }
}

void llParser::expect(char expected)
{
	if (currentChar != expected)
	{
		throw ParseException(std::string("Expected '") + expected + "' but found '" + currentChar + "'");
	}
}
