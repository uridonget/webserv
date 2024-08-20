/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pipe.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/19 17:58:05 by sangyhan          #+#    #+#             */
/*   Updated: 2024/08/20 15:33:27 by sangyhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Pipe.hpp"


Pipe::Pipe(int inputFd, int outputFd) : Buffer(outputFd) {
    this->pid = -1;
    this->inputFd = inputFd;
    error = false;
    procEnd = false;
    readEnd = false;
    outClosed = false;
    inClosed = false;
    std::cout << "----[OPEN]----\n";
    std::cout << inputFd << ' ' << outputFd << std::endl;
}

Pipe::~Pipe() {
}

int Pipe::whoAmI() {
    return 3;
}

const int &Pipe::getServerFd()
{
    return serverFd;
}

void Pipe::setServerFd(int fd)
{
    serverFd = fd;    
}

const bool &Pipe::getError()
{
    return error;
}

void Pipe::setError(bool code)
{
    this->error = code;
}

bool Pipe::End()
{
    if (procEnd == true && readEnd == true)
    {
        return true;
    }
    else
    {
        return false;
    }
}
void Pipe::setProcEnd(bool code)
{
    procEnd = code;
}

void Pipe::setReadEnd(bool code)
{
    readEnd = code;
}

int Pipe::autoWrite(size_t size)
{
    if (index + size > writeBuffer.size())
	{
		size = writeBuffer.size() - index;
	}
	for (size_t i = 0; i < size; i++) {
		cBuffer[i] = writeBuffer[index + i];
	}
	cBuffer[size + 1] = '\0';
	int writtenSize = write(inputFd, cBuffer, size);
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
}

const int &Pipe::getInputFd()
{
    return inputFd;
}

const pid_t &Pipe::getPid()
{
    return pid;
}

void Pipe::setPid(pid_t pid)
{
    this->pid = pid;
}

void Pipe::closeInput()
{
    if (inClosed == false)
    {
        close(inputFd);
        std::cout << "close " << inputFd << std::endl;
        inClosed = true;
    }
}

void Pipe::closeOutput()
{
    if (outClosed == false)
    {
        close(fd);
        std::cout << "close " << fd << std::endl;
        outClosed = true;
    }
}