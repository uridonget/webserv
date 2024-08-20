/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebservUtils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 16:46:15 by heolee            #+#    #+#             */
/*   Updated: 2024/08/20 15:29:44 by sangyhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Webserv.hpp"

/*
message : 1
file    : 2
pipe    : 3
*/
int Webserv::isMessage(int bufferIdx) {
	return bufferList[bufferIdx]->whoAmI();
}

void Webserv::closeSocket(int bufferIdx) {
	close(bufferList[bufferIdx]->getFd());
	Buffer *client = bufferList[bufferIdx];
	std::list<Buffer *> bufList = static_cast<Message *>(bufferList[bufferIdx])->getResourceList();
	for (size_t i = 0; i < bufferList.size(); i++)
	{
		if (bufList.empty())
		{
			break;
		}
		if (bufList.front() == bufferList[i])
		{
			int serverFd;
			if (bufferList[i]->whoAmI() == 2)
			{
				serverFd = (static_cast<File *>(bufferList[i]))->getServerFd();
				close((bufferList[i])->getFd());
			}
			else
			{
				serverFd = (static_cast<Pipe *>(bufferList[i]))->getServerFd();
				Pipe *pipe = static_cast<Pipe *>(bufferList[i]);
				pipe->closeInput();
				pipe->closeOutput();
			}
			std::map<int, Server>::iterator server = serverList.find(serverFd);
			if (server != serverList.end())
				server->second.deleteRequestByFile(bufList.front());
			delete bufferList[i]; // delete File
			bufferList.erase(bufferList.begin() + i);
			i -= 1;
			bufList.erase(bufList.begin());
		}
	}
	if (!bufList.empty())
	{
		std::cout << "erase fail!!!!" << std::endl;
	}
	for (size_t i = 0; i < bufferList.size(); i++)
	{
		if (bufferList[i] == client)
		{
			delete bufferList[i]; // delete Message
			bufferList.erase(bufferList.begin() + i);		
			break;
		}
	}
}

void Webserv::closeFile(int bufferIdx) {
	close(bufferList[bufferIdx]->getFd());
}

void Webserv::successResponse(int bufferIdx) {
	struct kevent clientEvent;
	EV_SET(&clientEvent, bufferList[bufferIdx]->getFd(), EVFILT_WRITE, EV_DELETE , 0, 0, NULL);
	changeList.push_back(clientEvent);
}

void Webserv::successFileWrite(int bufferIdx) {
	close(bufferList[bufferIdx]->getFd());
	delete bufferList[bufferIdx];
	bufferList.erase(bufferList.begin() + bufferIdx);
}
