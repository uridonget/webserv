/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Buffer.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: heolee <heolee@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 16:43:48 by heolee            #+#    #+#             */
/*   Updated: 2024/06/25 16:43:51 by heolee           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Buffer.hpp"

Buffer::Buffer(int fd) : fd(fd) {}

Buffer::~Buffer() {}

Buffer::Buffer(const Buffer& other) {
	this->fd = other.fd;
	this->readBuffer = other.readBuffer;
	this->writeBuffer = other.writeBuffer;
}

Buffer& Buffer::operator = (const Buffer& other) {
	if (this != &other) {
		this->fd = other.fd;
		this->readBuffer = other.readBuffer;
		this->writeBuffer = other.writeBuffer;
	}
	return *this;
}

int Buffer::whoAmI() {
	return 0;
}

int Buffer::getFd() const {
	return this->fd;
}

std::vector<char>& Buffer::getReadBuffer() {
	return this->readBuffer;
}

std::vector<char>& Buffer::getWriteBuffer() {
	return this->writeBuffer;
}
std::vector<char>::iterator Buffer::getWriteIt() const {
	return this->writeIt;
}

void Buffer::setWriteIt(std::vector<char>::iterator it) {
	this->writeIt = it;
}
