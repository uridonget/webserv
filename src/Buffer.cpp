/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Buffer.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 16:43:48 by heolee            #+#    #+#             */
/*   Updated: 2024/08/20 13:13:14 by sangyhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Buffer.hpp"

Buffer::Buffer(int fd) : fd(fd) {
	index = 0;
}

Buffer::~Buffer() {}

Buffer::Buffer(const Buffer& other) {
	this->fd = other.fd;
	this->readBuffer = other.readBuffer;
	this->writeBuffer = other.writeBuffer;
	index = 0;
}

Buffer& Buffer::operator = (const Buffer& other) {
	if (this != &other) {
		this->fd = other.fd;
		this->readBuffer = other.readBuffer;
		this->writeBuffer = other.writeBuffer;
		this->index = other.index;
	}
	return *this;
}

/*
0 : buffer (should not be existed)
1 : message
2 : file
*/

int Buffer::getFd() const {
	return this->fd;
}

std::vector<char>& Buffer::getReadBuffer() {
	return this->readBuffer;
}

std::vector<char>& Buffer::getWriteBuffer() {
	return this->writeBuffer;
}

#include <unistd.h>

bool Buffer::writeBufEmpty()
{
	if (index == writeBuffer.size())
	{
		return true;
	}
	else
	{
		return false;
	}
}

int Buffer::autoWrite(size_t size)
{
	if (index + size > writeBuffer.size())
	{
		size = writeBuffer.size() - index;
	}
	for (size_t i = 0; i < size; i++) {
		cBuffer[i] = writeBuffer[index + i];
	}
	cBuffer[size + 1] = '\0';
	int writtenSize = write(fd, cBuffer, size);
	if (writtenSize > 0)
	{
		index += writtenSize;
		if (index > writeBuffer.size() / 2)
		{
			writeBuffer.erase(writeBuffer.begin(), writeBuffer.begin() + index);
			index = 0;
		}
	}
	return writtenSize;
	return -1;
}