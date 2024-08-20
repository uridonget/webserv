/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   File.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 16:44:02 by heolee            #+#    #+#             */
/*   Updated: 2024/08/09 20:10:16 by sangyhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/File.hpp"

File::File(int fd) : Buffer(fd) {
    error = false;
}

File::File(int fd, std::string filename) : Buffer(fd), filename(filename) {
    error = false;
}

File::~File() {

}

int File::whoAmI() {
    return 2;
}

const int &File::getServerFd()
{
    return serverFd;
}

void File::setServerFd(int fd)
{
    serverFd = fd;    
}

const std::string &File::getFilename() const
{
    return filename;
}

void File::setError(bool code)
{
    error = code;
}

const bool &File::getError()
{
    return error;
}