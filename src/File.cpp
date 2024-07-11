/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   File.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 16:44:02 by heolee            #+#    #+#             */
/*   Updated: 2024/07/11 15:47:57 by sangyhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/File.hpp"

File::File(int fd) : Buffer(fd) {}

File::File(int fd, std::string filename) : Buffer(fd), filename(filename) {}

File::~File() {}

int File::whoAmI() {
    return 2;
}

const std::string &File::getFilename() const
{
    return filename;
}