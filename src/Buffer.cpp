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

int Buffer::whoAmI() {
    return 0;
}

int Buffer::getFd() const {
    return this->fd;
}

std::vector<char> Buffer::getBuffer() const {
    return this->buffer;
}
