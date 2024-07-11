/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 21:09:51 by sangyhan          #+#    #+#             */
/*   Updated: 2024/07/11 15:46:44 by haejeong         ###   ########.fr       */
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
	if (client->getHeaderFlag() == false) { // header end 발견 X
		pos = findEnd(buf, append, size);
		if (pos != RequestParser::npos) { // header end 발견!
			client->setHeaderFlag(true);
			client->setHeaderEnd(pos);
		} else { // header end 없어요
			client->setHeaderFlag(false);
			client->setHeaderEnd(RequestParser::npos);
			// return (RequestParser::npos);
		}
	} else { // header end 발견 O
		buf.insert(buf.end(), append, append + size);
		pos = client->getHeaderEnd();
	}
	endHeader = pos;
	if (client->getHeaderFlag() == true) {
		size_t contentLen = client->getContentLength();
		if (contentLen == 0) { // 아직 안 찾아봄
			std::string content_header = "Content-Length:";
			size_t content_len_pos = kmp(buf, content_header, 0);
			if (content_len_pos == RequestParser::npos) { 
				// content length 못찾음
				client->setHeaderFlag(false);
				client->setHeaderEnd(RequestParser::npos);
				return (pos);
			} else { 
				// content length 찾았음
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
				if (contentLen >= 0 && pos + 4 + contentLen <= buf.size()) {
					// message done
					client->setHeaderFlag(false);
					client->setHeaderEnd(RequestParser::npos);
					return (pos + contentLen);
				} else {
					// need more body
					return (RequestParser::npos);
				}
			}
		} else if (contentLen == RequestParser::npos) { // 찾아봤는데 없음
			client->setHeaderFlag(false);
			client->setHeaderEnd(RequestParser::npos);
			return (pos);
		} else { // 찾았음
			client->setHeaderFlag(false);
			client->setHeaderEnd(RequestParser::npos);
			return (pos + contentLen);
		}
	}
	// header not done
	return (RequestParser::npos);
}
