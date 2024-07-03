/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   llParser.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/02 14:08:20 by sangyhan          #+#    #+#             */
/*   Updated: 2024/07/03 15:34:46 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/llParser.hpp"

class ParseException : public std::runtime_error
{
public:
    ParseException(const std::string &msg) : std::runtime_error(msg) {}
};

llParser::llParser(std::vector<char> buffer, size_t endHeader) : stream(std::string(buffer.begin(), buffer.begin() + endHeader + 4)), currentChar(NULL_CHAR)
{
    nextChar();
}

HttpRequest llParser::parse()
{
	HttpRequest request;
	std::string method = parseToken();
	if (method == "GET") {
		request.method = GET;
	}
	else if (method == "POST") {
		request.method = POST;
	}
	else if (method == "DELETE") {
		request.method = DELETE;
	}
	consumeSP();
	request.url = parseToken();
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
    if (request.headers.find("Content-Length") != request.headers.end())
    {
        request.contentLength = request.headers["Content-Length"];
    }
    return request;
}

void llParser::nextChar()
{
    stream.get(currentChar);
    if (stream.eof())
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
