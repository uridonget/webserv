/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 16:44:16 by heolee            #+#    #+#             */
/*   Updated: 2024/08/01 14:13:47 by sangyhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Message.hpp"

Message::Message(int fd) : 
Buffer(fd), 
headerFlag(false), 
headerEnd(18446744073709551615UL), 
contentLength(18446744073709551615UL),
chunkFlag(false),
chunkStart(0) {
    status = ALIVE;
}

Message::~Message() {}

int Message::whoAmI() {
    return 1;
}

void Message::clearFlag()
{
    
}

void Message::setHeaderFlag(bool flag)
{
    headerFlag = flag;
}

const bool &Message::getHeaderFlag() const
{
    return headerFlag;
}

void Message::setHeaderEnd(size_t pos)
{
    headerEnd = pos;
}

const size_t &Message::getHeaderEnd() const
{
    return headerEnd;
}

void Message::setContentLength(size_t len) {
    contentLength = len;
}

const size_t &Message::getContentLength() const {
    return contentLength;
}

void Message::setChunkFlag(bool flag) {
    chunkFlag = flag;
}

const bool &Message::getChunkFlag() const {
    return chunkFlag;
}

void Message::setChunkStart(size_t pos) {
    chunkStart = pos;
}

const size_t &Message::getChunkStart() const {
    return chunkStart;
}

void Message::addResource(Buffer *buf)
{
    resourceList.push_back(buf);
}

void Message::deleteResource(Buffer *buf)
{
    std::list<Buffer*>::iterator it = std::find(resourceList.begin(), resourceList.end(), buf);
    if (it != resourceList.end())
    {
        resourceList.erase(it);
    }
}

const std::list<Buffer *> &Message::getResourceList() const
{
    return resourceList;
}

std::vector<char> & Message::getChunkBuffer() {
    return chunkBuffer;
}

const enum CLIENT_STATUS &Message::getStatus()
{
    return status;
}

void Message::setStatus(enum CLIENT_STATUS status)
{
    this->status = status;
}
