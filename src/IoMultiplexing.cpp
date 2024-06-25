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

// this file is main loop 

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <map>
#include <vector>
#include "../include/Webserv.hpp"

static void set_nonblocking(int sockfd) {
	fcntl(sockfd, F_SETFL, O_NONBLOCK);
}

static int init_server(int server_fd) {
	int opt = 1;
	struct sockaddr_in address;

	set_nonblocking(server_fd);
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
		throw RuntimeException("setsockopt");
	}

	std::memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(8080);                                              // 여기 변수로 꽂아야함

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
		throw RuntimeException("bind");
	}

	if (listen(server_fd, 10) == -1) {
		throw RuntimeException("listen");
	}

	int kq = kqueue();
	if (kq == -1) {
		throw RuntimeException("kqueue");
	}

	return kq;
}


