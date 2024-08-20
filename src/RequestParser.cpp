/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 21:09:51 by sangyhan          #+#    #+#             */
/*   Updated: 2024/07/29 10:58:49 by sangyhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/RequestParser.hpp"

size_t RequestParser::kmp(std::vector<char> &buf, std::string &target, size_t start)
{
	size_t size1 = buf.size();
	size_t size2 = target.size();
	size_t j = 0;
	std::vector<int> dp;

	dp.resize(target.size());
	std::fill_n(dp.begin(), target.size(), 0);
	for (size_t i = 1; i < size2; i++) {
		while (j > 0 && target[i] != target[j]) {
			j = dp[j - 1];
		}
		if (target[i] == target[j]) {
			dp[i] = ++j;
		}
	}
	j = 0;
	for (size_t i = start; i < size1; i++) {
		while (j > 0 && buf[i] != target[j]) {
			j = dp[j - 1];
		}
		if (buf[i] == target[j]) {
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

	if (buf.size() + size < 4) {
		buf.reserve(size);
		buf.insert(buf.end(), append, append + size);
		return (RequestParser::npos);
	}
	else if (buf.size() < 3) {
		start = 0;
	}
	else {
		start = buf.size() - 3;
	}
	buf.reserve(size);
	buf.insert(buf.end(), append, append + size);
	res = kmp(buf, target, start);
	if (res == RequestParser::npos) {
		return (RequestParser::npos);
	}
	else {
		return (res);
	}
}

bool RequestParser::chunkParsing(std::vector<char> &buf, Message *client, bool & endFlag) {
	size_t chunkStart = client->getChunkStart();
	size_t bufSize = buf.size();

	if (chunkStart >= bufSize) {
		return false;
	}
	size_t pos = chunkStart;
	while (pos < bufSize && buf[pos] != '\r') {
		if (!(isHexDigit(buf[pos]))) {
			return false;
		}
		pos++;
	}
	if (pos + 1 >= bufSize || buf[pos + 1] != '\n') {
		return false;
	}
	std::string hexNum(buf.begin() + chunkStart, buf.begin() + pos);
	int num = hexToDecimal(hexNum);
	if (bufSize < pos + num + 4) {
		return false;
	}
	if (buf.size() < chunkStart + 4 + num + hexNum.size()) {
		return false;
	}
	if (buf[pos + num + 2] != '\r' || buf[pos + num + 3] != '\n') {
		return false;
	}
	client->getChunkBuffer().insert(client->getChunkBuffer().end(), buf.begin() + pos + 2, buf.begin() + pos + num + 2);
	client->setChunkStart(pos + num + 4);
	if (num == 0) {
		endFlag = true;
		return false;
	}
	return true;
}

size_t RequestParser::checkEnd(Message *client, char *append, size_t size, size_t & endHeader)
{
	std::vector<char> &buf = client->getReadBuffer();
	size_t pos;
	size_t contentLen;
	if (client->getHeaderFlag() == false)
	{
		pos = findEnd(buf, append, size);
		if (pos != RequestParser::npos)
		{ 
			request.chunked = true;
			client->setHeaderFlag(true);
			client->setHeaderEnd(pos);
			std::string chunk_header = "Transfer-Encoding: chunked";
			if (client->getChunkFlag() == false && kmp(buf, chunk_header, 0) != RequestParser::npos) {
				client->setChunkFlag(true);
				client->setChunkStart(pos + 4);
			}
			std::string content_header = "Content-Length:";
			size_t content_len_pos = kmp(buf, content_header, 0);
			if (content_len_pos != RequestParser::npos) {
				int content_len_tail = content_len_pos;
				while (buf[content_len_tail] != '\r') {
					content_len_tail++;
				}
				std::string temp;
				for (int k = content_len_pos + content_header.size(); k < content_len_tail; k++) {
					temp += buf[k];
				}
				contentLen = std::atoi(temp.c_str());
				client->setContentLength(contentLen);
			}
		}
		else 
		{ 
			client->setHeaderFlag(false);
			client->setHeaderEnd(RequestParser::npos);
			return (RequestParser::npos);
		}
	}
	else
	{
		buf.reserve(size);
		buf.insert(buf.end(), append, append + size);
		pos = client->getHeaderEnd();
	}
	endHeader = pos;
	contentLen = client->getContentLength();
	if (client->getChunkFlag())
	{
		bool endFlag = false;
		while (chunkParsing(buf, client, endFlag)) {};
		if (endFlag == true) 
		{
			buf.erase(buf.begin() + endHeader + 4, buf.begin() + client->getChunkStart());
			buf.reserve(client->getChunkBuffer().size());
			buf.insert(buf.begin() + endHeader + 4, client->getChunkBuffer().begin(), client->getChunkBuffer().end());
			size_t end = endHeader + 4 + client->getChunkBuffer().size();
			client->getChunkBuffer().clear();
			client->setHeaderFlag(false);
			client->setHeaderEnd(RequestParser::npos);
			client->setContentLength(RequestParser::npos);
			client->setChunkFlag(false);
			client->setChunkStart(RequestParser::npos);
			return (end - 4);
		} 
		else 
		{
			return (RequestParser::npos);
		}
	}
	else if (contentLen == RequestParser::npos)
	{
		client->setHeaderFlag(false);
		client->setHeaderEnd(RequestParser::npos);
		client->setContentLength(RequestParser::npos);
		return (pos);
	}
	else
	{
		if (contentLen >= 0 && pos + 4 + contentLen <= buf.size()) 
		{
			// message done
			client->setHeaderFlag(false);
			client->setHeaderEnd(RequestParser::npos);
			client->setContentLength(RequestParser::npos);
			return (pos + contentLen);
		}
		else 
		{
			// need more body
			return (RequestParser::npos);
		}
	}
}
