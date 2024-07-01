/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IoMultiplexing.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: heolee <heolee@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 16:46:15 by heolee            #+#    #+#             */
/*   Updated: 2024/06/25 16:46:15 by heolee           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Webserv.hpp"

void setNonblock(int fd) {
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
		throw RuntimeException("fcntl");
	}
}

int Webserv::isMessage(int bufferIdx) {
	// Message 1
	// File 2
	return bufferList[bufferIdx]->whoAmI();
}

void Webserv::closeSocket(int bufferIdx) {
	struct kevent clientEvent;
	EV_SET(&clientEvent, bufferList[bufferIdx]->getFd(), EVFILT_READ, EV_DELETE, 0, 0, NULL);
	changeList.push_back(clientEvent);
	struct kevent clientEvent1;
	EV_SET(&clientEvent1, bufferList[bufferIdx]->getFd(), EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
	changeList.push_back(clientEvent1);
	close(bufferList[bufferIdx]->getFd());
	delete bufferList[bufferIdx];
	bufferList.erase(bufferList.begin() + bufferIdx);
}

void Webserv::closeFile(int bufferIdx) {
	struct kevent fileEvent;
	EV_SET(&fileEvent, bufferList[bufferIdx]->getFd(), EVFILT_READ, EV_DELETE, 0, 0, NULL);
	changeList.push_back(fileEvent);
	close(bufferList[bufferIdx]->getFd());

	// push server

}

void Webserv::successResponse(int bufferIdx) {
	struct kevent clientEvent;
	EV_SET(&clientEvent, bufferList[bufferIdx]->getFd(), EVFILT_WRITE, EV_DELETE , 0, 0, NULL);
	changeList.push_back(clientEvent);
}

void Webserv::successFileWrite(int bufferIdx) {
	struct kevent clientEvent;
	EV_SET(&clientEvent, bufferList[bufferIdx]->getFd(), EVFILT_WRITE, EV_DELETE , 0, 0, NULL);
	changeList.push_back(clientEvent);
	delete bufferList[bufferIdx];
	bufferList.erase(bufferList.begin() + bufferIdx);
	close(bufferList[bufferIdx]->getFd());
}
