/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiParser.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/19 15:18:34 by sangyhan          #+#    #+#             */
/*   Updated: 2024/07/22 11:20:26 by sangyhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/CgiParser.hpp"

class ParseException : public std::runtime_error
{
public:
    ParseException(const std::string &msg) : std::runtime_error(msg) {}
};

CgiParser::CgiParser(const std::vector<char> &buffer, size_t endHeader) : stream(std::string(buffer.begin(), buffer.begin() + endHeader + 4)), currentChar(NULL_CHAR)
{
    nextChar();
}

HttpRequest CgiParser::parse(int &code, std::string &reason)
{
	HttpRequest request;

    std::string status = parseFieldName();
	if (status != "Status")
        throw ParseException("Expected Status");
    expect(':');
    nextChar();
    consumeOWS();
    code = std::atoi(parseToken().c_str());
	consumeSP();
    while (currentChar != CR  && currentChar != LF  && currentChar != NULL_CHAR) {
        reason += parseToken();
        if (currentChar == SP) {
            reason += SP;
            consumeSP();
        }
    }
	consumeNL();
	while (currentChar != NULL_CHAR && currentChar != CR && currentChar != LF)
	{
		std::pair<std::string, std::string> header = fieldline();
		request.headers[header.first] = header.second;
	}
    consumeNL();
    expect('\0');
    return request;
}

void CgiParser::nextChar()
{
    stream.get(currentChar);
    if (stream.eof())
    {
        currentChar = NULL_CHAR;
    }
}

void CgiParser::consumeSP()
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

void CgiParser::consumeNL()
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
    else if (currentChar == LF)
    {
        nextChar();
    }
    else
    {
        throw ParseException("Expected CRLF");
    }
}

std::string CgiParser::parseToken()
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

std::pair<std::string, std::string> CgiParser::fieldline()
{
    std::string name = parseFieldName();
    expect(':');
    nextChar();
    consumeOWS();
    std::string value = parseFieldValue();
    consumeNL();
    return (std::make_pair(name, value));
}

std::string CgiParser::parseFieldName()
{
    std::string name;
    while (currentChar != CR && currentChar != LF && currentChar != NULL_CHAR && currentChar != SP && currentChar != '\t' && currentChar != ':')
    {
        name += currentChar;
        nextChar();
    }
    return name;
}

std::string CgiParser::parseFieldValue()
{
    std::string value;
    while (currentChar != CR && currentChar != LF && currentChar != NULL_CHAR)
    {
        value += currentChar;
        nextChar();
    }
    return value;
}

void CgiParser::consumeOWS()
{
    while (currentChar == ' ')
    {
        nextChar();
    }
}

void CgiParser::expect(char expected)
{
	if (currentChar != expected)
	{
		throw ParseException(std::string("Expected '") + expected + "' but found '" + currentChar + "'");
	}
}
