/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebservUtils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 16:46:15 by heolee            #+#    #+#             */
/*   Updated: 2024/07/11 16:54:36 by sangyhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Webserv.hpp"

int Webserv::isMessage(int bufferIdx) {
	// Message 1
	// File 2
	return bufferList[bufferIdx]->whoAmI();
}

void Webserv::closeSocket(int bufferIdx) {
	struct kevent clientEvent1;
	EV_SET(&clientEvent1, bufferList[bufferIdx]->getFd(), EVFILT_READ, EV_DELETE, 0, 0, NULL);
	changeList.push_back(clientEvent1);
	struct kevent clientEvent2;
	EV_SET(&clientEvent2, bufferList[bufferIdx]->getFd(), EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
	changeList.push_back(clientEvent2);
	close(bufferList[bufferIdx]->getFd());
	delete bufferList[bufferIdx];
	bufferList.erase(bufferList.begin() + bufferIdx);
}

void Webserv::closeFile(int bufferIdx) {
	struct kevent fileEvent;
	EV_SET(&fileEvent, bufferList[bufferIdx]->getFd(), EVFILT_READ, EV_DELETE, 0, 0, NULL);
	changeList.push_back(fileEvent);
	close(bufferList[bufferIdx]->getFd());
}

void Webserv::successResponse(int bufferIdx) {
	struct kevent clientEvent;
	EV_SET(&clientEvent, bufferList[bufferIdx]->getFd(), EVFILT_WRITE, EV_DELETE , 0, 0, NULL);
	changeList.push_back(clientEvent);
}

void Webserv::successFileWrite(int bufferIdx) {
	close(bufferList[bufferIdx]->getFd());
	struct kevent clientEvent;
	EV_SET(&clientEvent, bufferList[bufferIdx]->getFd(), EVFILT_WRITE, EV_DELETE , 0, 0, NULL);
	changeList.push_back(clientEvent);
	delete bufferList[bufferIdx];
	bufferList.erase(bufferList.begin() + bufferIdx);
}
