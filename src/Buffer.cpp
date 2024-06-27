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

Buffer::Buffer() {}

Buffer::~Buffer() {}

int Buffer::getFd() const {
    return this->fd;
}

std::vector<char> Buffer::getBuffer() const {
    return this->buffer;
}
