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

// #include "../include/Webserv.hpp"

// void Webserv::ioMultiplexing() {



// 	while (true) {
// 		int nevents = kevent(kq, &changeList[0], changeList.size(), eventList, 10, NULL);
// 		if (nevents == -1) {
// 			throw RuntimeException("kevent error");
// 		}
// 		changeList.clear();
// 		for (int i = 0; i < nevents; i++) {
// 			if (eventList[i].flags == EV_ERROR) {
// 				// 에러 체크
// 			}
// 			else if ("새 클라이언트 판별") {
// 				// 새로운 클라이언트
// 			}
// 			else if ("이미 있는 클라이언트") {
// 				if (eventList[i].filter == EVFILT_READ) {

// 					for (int j = 0; j < bufferList.size(); j++) {
// 						if (bufferList[j].getFd() == eventList[j].ident) {

// 							// Message == 1
// 							if (bufferList[j].whoAmI() == 1) {}

// 							// File == 2
// 							else if (bufferList[j].whoAmI() == 2) {
// 								if ("read end") {

// 									struct kevent newEvent;
// 									EV_SET(&newEvent, eventList[j].ident, EVFILT_VNODE, EV_DELETE, 0, 0, NULL);
// 									// changeList.push_back();

// 									// ??
// 									// close(eventList[j].ident);

// 									// makeResponse 해야함 
// 									bufferList[j].getBuffer();

// 									bufferList.erase(bufferList.begin() + j);

// 									//이제 클라이언트에 write 이벤트로 추가 해야함

// 								}

// 								//근데 파일 읽는데 read end no 는 어떤 경우임? 
// 								else if (!"read end") {}
// 							}

// 							break;
// 						}
// 					}
// 				}

// 				else if (eventList[i].filter == EVFILT_WRITE) {}

// 			}
// 		}
// 	}
// }
