/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 21:09:51 by sangyhan          #+#    #+#             */
/*   Updated: 2024/07/01 16:06:33 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/RequestParser.hpp"

size_t RequestParser::kmp(std::vector<char> &buf, std::string &target, size_t start)
{
	int size1 = buf.size();
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
	for (int i = 0; i < size1; i++)
    {
		while (j > 0 && buf[i] != target[j])
        {
            j = dp[j - 1];
        }
        if (buf[i] == target[j])
        {
            j++;
        }
        if (j == size2) {
			return (i - target.size() + 1);
		}
	}
	return RequestParser::npos;
}

size_t RequestParser::findEnd(std::vector<char> &buf, char *append, size_t size)
{
    std::string target = "\r\n\r\n";
    size_t start = 0;
    size_t res = 0;

    if (buf.size() + size < 4)
    {
        return (RequestParser::npos);
    }
    else if (buf.size() < 3)
    {
        start = 0;
    }
    else
    {
        start = buf.size() - 3;
    }
    buf.insert(buf.end(), append, append + size);
    res = kmp(buf, target, start);
    if (res == RequestParser::npos)
    {
        return (RequestParser::npos);
    }
    else
    {
        return (res);
    }
}

size_t RequestParser::checkEnd(std::vector<char> &buf, char *append, size_t size, size_t & endHeader)
{
    size_t pos = findEnd(buf, append, size);
    endHeader = pos;
    if (pos != RequestParser::npos)
    {
        std::string content_header = "Content-Length:";
        size_t content_len_pos = kmp(buf, content_header, 0);
        if (content_len_pos == RequestParser::npos)
        {
            // no need content
            // message done
            return (pos);
        }
        else
        {
            // need content
            int content_len_tail = content_len_pos; 
            while (buf[content_len_tail] != '\r')
            {
                content_len_tail++;
            }
            std::string temp;
            for (int k = content_len_pos + content_header.size(); k < content_len_tail; k++)
            {
                temp += buf[k];
            }
            int content_len = std::atoi(temp.c_str());
            if (content_len >= 0 && pos + 4 + content_len >= buf.size())
            {
                // message done
                
                return (pos + content_len);
            }
            else
            {
                // need more body
                return (RequestParser::npos);
            }
        }
    }
    // header not done
    return (RequestParser::npos);
}

struct HttpRequest RequestParser::requestParsing(std::vector<char> fullRequest, size_t endIndex, size_t & endHeader) {
    struct HttpRequest parsedRequest;
    
    std::string request(fullRequest.begin(), fullRequest.begin() + endIndex + 1);
    
    std::istringstream iss(request);
    std::string line;
    std::vector<std::string> tokens;
    std::string temp;

    std::getline(iss, line);
    tokens.clear();
    std::istringstream lineStream(line);
    while (lineStream >> temp) {
        tokens.push_back(temp);
    }
    if (tokens.size() != 3) {
        throw RuntimeException("invalid http request header1");
    }
    parsedRequest.method = tokens[0];
    parsedRequest.url = tokens[1];
    parsedRequest.httpVersion = tokens[2];
    while (std::getline(iss, line)) {
        tokens.clear();
        std::istringstream lineStream(line);
        while (lineStream >> temp) {
            tokens.push_back(temp);
        }
        if (tokens[0] == "Host:") {
            if (tokens.size() != 2)
                throw RuntimeException("invalid http request header2");
            parsedRequest.host = tokens[1];
        }
        else if (tokens[0] == "User-Agent:") {
            std::string agent = "";
            for (int i=1; i < tokens.size(); i++) {
                agent += tokens[i];
                agent += ' ';
            }
            parsedRequest.userAgent = agent;
        }
        else if (tokens[0] == "Accept:") {
            if (tokens.size() != 2) {
                throw RuntimeException("invalid http request header4");
            }
            parsedRequest.accept = tokens[1];
        }
        else if (tokens[0] == "Content-Length:") {
            if (tokens.size() != 2) {
                throw RuntimeException("invalid http request header4");
            }
            parsedRequest.contentLength = tokens[1];
        }
        else if (line.length() == 0) 
            break ;
    }
    for (int i=0; endHeader + i < endIndex; i++) {
        parsedRequest.body.push_back(request[i]);
    }
    return parsedRequest;
}
