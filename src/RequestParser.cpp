/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 21:09:51 by sangyhan          #+#    #+#             */
/*   Updated: 2024/07/09 19:44:33 by haejeong         ###   ########.fr       */
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
	for (int i = 1; i < size2; i++) {
		while (j > 0 && target[i] != target[j]) {
			j = dp[j - 1];
		}
		if (target[i] == target[j]) {
			dp[i] = ++j;
		}
	}
	j = 0;
	for (int i = 0; i < size1; i++) {
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
		return (RequestParser::npos);
	}
	else if (buf.size() < 3) {
		start = 0;
	}
	else {
		start = buf.size() - 3;
	}
	buf.insert(buf.end(), append, append + size);
	res = kmp(buf, target, start);
	if (res == RequestParser::npos) {
		return (RequestParser::npos);
	}
	else {
		return (res);
	}
}

size_t RequestParser::checkEnd(Message *client, char *append, size_t size, size_t & endHeader)
{
	std::vector<char> &buf = client->getReadBuffer();
	size_t pos;
	if (client->getHeaderFlag() == false)
	{
		// std::cout << "header not end!" << std::endl;
		pos = findEnd(buf, append, size);
		if (pos != RequestParser::npos)
		{
			client->setHeaderFlag(true);
			// std::cout << "Message header end position set : " << pos << std::endl;
			client->setHeaderEnd(pos);
		}
		else
		{
			client->setHeaderFlag(false);
			client->setHeaderEnd(RequestParser::npos);
		}
	}
	else 
	{
		// std::cout << "already header end!" << std::endl;
		buf.insert(buf.end(), append, append + size);
		pos = client->getHeaderEnd();
	}
	endHeader = pos;
	if (pos != RequestParser::npos) {
		std::string content_header = "Content-Length:";
		size_t content_len_pos = kmp(buf, content_header, 0);
		if (content_len_pos == RequestParser::npos) 
		{
			// no need content
			// message done
			client->setHeaderFlag(false);
			client->setHeaderEnd(RequestParser::npos);
			return (pos);
		}
		else {
			// need content
			int content_len_tail = content_len_pos; 
			while (buf[content_len_tail] != '\r') {
				content_len_tail++;
			}
			std::string temp;
			for (int k = content_len_pos + content_header.size(); k < content_len_tail; k++) {
				temp += buf[k];
			}
			int content_len = std::atoi(temp.c_str());
			if (content_len >= 0 && pos + 4 + content_len <= buf.size()) {
				// message done
				client->setHeaderFlag(false);
				client->setHeaderEnd(RequestParser::npos);
				return (pos + content_len);
			}
			else {
				// need more body
				return (RequestParser::npos);
			}
		}
	}
	// header not done
	return (RequestParser::npos);
}

// void RequestParser::setBody(HttpRequest & request, std::vector<char> & buf, size_t & endHeader, size_t & endIndex) {
// 	if (endHeader != endIndex) {
// 		request.body.reserve(request.body.size() + (endIndex - endHeader));
// 		request.body.insert(request.body.end(), buf.begin() + endHeader + 4, buf.begin() + endIndex + 4);
// 	}
// }
