/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MimeParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 20:45:54 by sangyhan          #+#    #+#             */
/*   Updated: 2024/07/06 13:44:02 by sangyhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/MimeParser.hpp"

size_t MimeParser::kmp(std::string &target)
{
    int size1 = buf.size() - index;
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
    for (int i = index; i < buf.size(); i++)
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
    if (now != buf.end())
    {
        index++;
        now++;
    }
    if (now == buf.end())
    {
        currentChar = NULL_CHAR;
    }
}

int MimeParser::consumeCRLF()
{
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
            return (-1);
        }
    }
    else
    {
        return (-1);
    }
}

int MimeParser::consumeMimeEnd()
{
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
            return (-1);
        }
    }
    else
    {
        return (-1);
    }
}

int MimeParser::consumeBoder()
{
    for (int i = 0; i < boder.size(); i++)
    {
        if (currentChar == boder[i])
        {
            nextChar();
        }
        else
        {
            return (-1);
        }
    }
    return (0);
}

void MimeParser::deleteFileAll(std::vector<Buffer *> &request)
{
    for (int i = 0; i < request.size(); i++)
    {
        File *temp = dynamic_cast<File *>(request[i]);
        delete temp;
    }
}

int MimeParser::fieldLine(std::vector<std::pair<std::string, std::string> > &header)
{
    std::string name = parseFieldName();
    expect(':');
    nextChar();
    std::string value = parseFieldValue();
    consumeCRLF();
    header.push_back(std::make_pair(name, value));
    return (0);
}

std::string MimeParser::parseFieldName()
{
    std::string name;
    while (currentChar != CR && currentChar != LF && currentChar != NULL_CHAR && currentChar != ':')
    {
        name += currentChar;
        nextChar();
    }
    return name;
}

std::string MimeParser::parseFieldValue()
{
    std::string name;
    while (currentChar != CR && currentChar != LF && currentChar != NULL_CHAR)
    {
        name += currentChar;
        nextChar();
    }
    return name;
}

int MimeParser::consumeUntilBoder(std::vector<char> &content)
{
    size_t pos = kmp(boder);
    if (pos == npos)
    {
        return (-1);
    }
    if (pos > index)
    {
        content.insert(content.begin(), now, now + pos);
    }
    return (0);
}

int MimeParser::expect(char expected)
{
    if (currentChar != expected)
    {
        return (-1);
    }
    return (0);
}

MimeParser::MimeParser(std::string boder) : boder(boder), index(0) {}

std::vector<Buffer *> MimeParser::parse(std::string root)
{
    bool errorFlag = true;
    std::vector<std::pair<std::string, std::string> > header;
    int index = 0;
    while (errorFlag == false)
    {
        consumeBoder();
        while (currentChar != NULL_CHAR && currentChar != CR)
        {
            if (fieldLine(header) == -1)
            {
                errorFlag = true;
                break;
            }
        }
        if (errorFlag == true)
        {
            break;
        }
        std::vector<std::pair<std::string, std::string> >::iterator it;
        std::string contentHeader = "Content-Disposition";
        for (it = header.begin() ;it != header.end(); it++)
        {
            if (it->first == contentHeader)
            {
                break;
            }
        }
        if (it != header.end())
        {
            size_t pos = it->second.find_last_of("filename=");
            std::string filename = it->second.substr(pos + 9, std::string::npos);
            if (filename.size() > 0)
            {
                std::string path = root;
                path += "/upload/";
                path += filename;
                int fd = open(path.c_str(), O_RDWR);
                if (fd > 0)
                {
                    File *newfile = new File(fd);
                    request.push_back(static_cast<Buffer *>(newfile));
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
        consumeCRLF();
        if (request.size() > index)
        {
            consumeUntilBoder(request[request.size()]->getWriteBuffer());
        }
        consumeBoder();
        consumeCRLF();
        header.clear();
        index += 1;
    }
    if (errorFlag == false && consumeMimeEnd() == -1)
    {
        errorFlag = true;
    }
    else if (errorFlag == false && expect('\0') == -1)
    {
        errorFlag = true;
    }
    if (errorFlag == true)
    {
        deleteFileAll(request);
        request.clear();
    }
    return request;
}
