/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MimeParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 20:45:54 by sangyhan          #+#    #+#             */
/*   Updated: 2024/07/29 13:56:18 by sangyhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/MimeParser.hpp"

size_t MimeParser::kmp(std::string &target)
{
    int size2 = target.size();
    int j = 0;
    std::vector<int> dp;

    dp.resize(target.size());
    std::fill_n(dp.begin(), target.size(), 0);
    for (int i = 1; i < size2; i++)
    {
        while (j > 0 && target[i] != target[j])
        {
            j = dp[j - 1];
        }
        if (target[i] == target[j])
        {
            dp[i] = ++j;
        }
    }
    j = 0;
    for (size_t i = bufIndex; i < buf.size(); i++)
    {
        while (j > 0 && buf[i] != target[j])
        {
            j = dp[j - 1];
        }
        if (buf[i] == target[j])
        {
            j++;
        }
        if (j == size2)
        {
            return (i - target.size() + 1);
        }
    }
    return npos;
}

void MimeParser::nextChar()
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

void MimeParser::consumeOWS()
{
    while (currentChar == ' ')
    {
        nextChar();
    }
}

int MimeParser::consumeCRLF()
{
    int lastIndex = bufIndex;
    if (currentChar == CR)
    {
        nextChar();
        if (currentChar == LF)
        {
            nextChar();
            return (0);
        }
        else
        {
            bufIndex = lastIndex;
            return (-1);
        }
    }
    else
    {
        bufIndex = lastIndex;
        return (-1);
    }
}

int MimeParser::consumeMimeEnd()
{
    int lastIndex = bufIndex;
    if (currentChar == '-')
    {
        nextChar();
        if (currentChar == '-')
        {
            nextChar();
            return (0);
        }
        else
        {
            bufIndex = lastIndex;
            return (-1);
        }
    }
    else
    {
        bufIndex = lastIndex;
        return (-1);
    }
}

int MimeParser::consumeBoundary()
{
    int lastIndex = bufIndex;
    for (size_t i = 0; i < boundary.size(); i++)
    {
        if (currentChar == boundary[i])
        {
            nextChar();
        }
        else
        {
            bufIndex = lastIndex;
            return (-1);
        }
    }
    return (0);
}

void MimeParser::deleteFileAll(std::vector<Buffer *> &request)
{
    for (size_t i = 0; i < request.size(); i++)
    {
        File *temp = dynamic_cast<File *>(request[i]);
        close(temp->getFd());
        std::remove(temp->getFilename().c_str());
        delete temp;
    }
}

int MimeParser::fieldLine(std::vector<std::pair<std::string, std::string> > &header)
{
    std::string name = parseFieldName();
    if (expect(':') == -1)
        return -1;
    consumeOWS();
    std::string value = parseFieldValue();
    header.push_back(std::make_pair(name, value));
    while (true)
    {
        consumeOWS();
        if (expect(';') == -1)
            break;
        consumeOWS();
        if (currentChar == CR || currentChar == LF || currentChar == NULL_CHAR)
            break;
        name = "";
        value = "";
        while (currentChar != CR && currentChar != LF && currentChar != NULL_CHAR && currentChar != '=')
        {
            name += currentChar;
            nextChar();
        }
        if (expect('=') != -1)
        {
            bool dquote = false;
            if (currentChar == '\"')
            {
                nextChar();
                dquote = true;
            }
            while ((dquote == true && currentChar != NULL_CHAR)
                || (currentChar != CR && currentChar != LF && currentChar != NULL_CHAR && currentChar != ' ' && currentChar != ';'))
            {
                if (dquote == true && currentChar == '\"')
                {
                    break;
                }
                value += currentChar;
                nextChar();
            }
            if (currentChar == '\"')
                nextChar();
            consumeOWS();
        }
        header.push_back(std::make_pair(name, value));
    }
    consumeOWS();
    if (consumeCRLF() == -1)
        return -1;
    return (0);
}

std::string MimeParser::parseFieldName()
{
    std::string name;
    while (currentChar != CR && currentChar != LF && currentChar != NULL_CHAR && currentChar != ' ' && currentChar != ':')
    {
        name += currentChar;
        nextChar();
    }
    return name;
}

std::string MimeParser::parseFieldValue()
{
    std::string name;
    bool dquote = false;
    if (currentChar == '\"')
    {
        nextChar();
        dquote = true;
    }
    while ((dquote == true && currentChar != NULL_CHAR)
        || (currentChar != CR && currentChar != LF && currentChar != NULL_CHAR && currentChar != ' ' && currentChar != ';'))
    {
        if (dquote == true && currentChar == '\"')
        {
            break;
        }
        name += currentChar;
        nextChar();
    }
    if (currentChar == '\"')
        nextChar();
    return name;
}

int MimeParser::consumeUntilBoundary(bool fileopen)
{
    std::string target = "\r\n" + boundary;
    size_t pos = kmp(target);
    if (pos == npos)
    {
        return (-1);
    }
    if (pos > bufIndex && fileopen == true)
    {
        std::vector<char> &content = request[request.size() - 1]->getWriteBuffer();
        content.reserve(pos - bufIndex);
        content.insert(content.begin(), buf.begin() + bufIndex, buf.begin() + pos);
        bufIndex = pos - 1;
        nextChar();
        consumeCRLF();
    }
    else if (pos > bufIndex && fileopen == false)
    {
        bufIndex = pos - 1;
        nextChar();
        consumeCRLF();
    }
    return (0);
}

int MimeParser::expect(char expected)
{
    if (currentChar != expected)
    {
        return (-1);
    }
    nextChar();
    return (0);
}

MimeParser::MimeParser(std::string boundary, std::vector<char> &buf, size_t start, size_t end) : buf(buf)
{
    this->boundary = "--" + boundary;
    this->bufIndex = start;
    this->end = end;
    if (bufIndex < buf.size())
        currentChar = buf[bufIndex];
    else
        currentChar = NULL_CHAR;
}

std::vector<Buffer *> MimeParser::parse(std::string root)
{
    bool errorFlag = false;
    bool fileopen = false;
    std::vector<std::pair<std::string, std::string> > header;
    request.clear();
    if (consumeBoundary() == -1 || consumeCRLF() == -1)
    {
        errorFlag = true;
    }
    while (errorFlag == false)
    {
        while (currentChar != NULL_CHAR && currentChar != CR)
        {
            if (fieldLine(header) == -1)
            {
                errorFlag = true;
                break;
            }
        }
        std::vector<std::pair<std::string, std::string> >::iterator it;
        std::string contentHeader = "filename";
        for (it = header.begin(); it != header.end(); it++)
        {
            if (it->first == contentHeader)
            {
                break;
            }
        }
        if (it != header.end())
        {
            std::string filename = it->second;
            if (filename.size() > 0)
            {
                std::string path = root;
                path += "/database/";
                path += filename;
                int fd = open(path.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
                if (fd > 0)
                {
                    File *newfile = new File(fd, path);
                    request.push_back(static_cast<Buffer *>(newfile));
                    fileopen = true;
                }
            }
            else
            {
                errorFlag = true;
                break;
            }
        }
        else
        {
            errorFlag = true;
            break;
        }
        if (consumeCRLF())
        {
            errorFlag = true;
            break;
        }
        consumeUntilBoundary(fileopen);
        fileopen = false;
        if (consumeBoundary() == -1)
        {
            errorFlag = true;
            break;
        }
        if (consumeMimeEnd() != -1)
        {
            if (consumeCRLF() == -1)
            {
                std::cerr << "CRLF ERROR" << std::endl;
                errorFlag = -1;
            }
            break;
        }
        if (consumeCRLF() == -1)
        {
            errorFlag = -1;
            break;
        }
        header.clear();
    }
    if (errorFlag == true)
    {
        deleteFileAll(request);
        request.clear();
    }
    return request;
}
