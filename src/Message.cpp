/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 16:44:16 by heolee            #+#    #+#             */
/*   Updated: 2024/07/11 19:45:48 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Message.hpp"

Message::Message(int fd) : 
Buffer(fd), 
headerFlag(false), 
headerEnd(18446744073709551615UL), 
contentLength(18446744073709551615UL),
chunkStart(0) {}

Message::~Message() {}

int Message::whoAmI() {
    return 1;
}

void Message::setHeaderFlag(bool flag)
{
    headerFlag = flag;
}

const bool Message::getHeaderFlag() const
{
    return headerFlag;
}

void Message::setHeaderEnd(size_t pos)
{
    headerEnd = pos;
}

const size_t Message::getHeaderEnd() const
{
    return headerEnd;
}

void Message::setContentLength(size_t len) {
    contentLength = len;
}

const size_t Message::getContentLength() const {
    return contentLength;
}

void Message::setChunkFlag(bool flag) {
    chunkFlag = flag;
}

const bool Message::getChunkFlag() const {
    return chunkFlag;
}

void Message::setChunkStart(size_t pos) {
    chunkStart = pos;
}

const size_t Message::getChunkStart() const {
    return chunkStart;
}
