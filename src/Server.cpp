/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 14:42:02 by haejeong          #+#    #+#             */
/*   Updated: 2024/06/27 11:27:19 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

Server::Server() : serverFd(0) {
	// std::cout << "Server default constructor called" << std::endl;
}

Server::~Server() {
	// std::cout << "Server destructor called" << std::endl;
}

void set_nonblock(int fd) {
    if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
        perror("fcntl(F_SETFL)");
        std::exit(EXIT_FAILURE);
    }
}

void Server::initServer(ServerConfig & config) {
	this->config = config;
	serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (serverFd < 0) {
        perror("socket");
        std::exit(EXIT_FAILURE);
    }
	set_nonblock(serverFd);
	int opt = 1;
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
	serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(this->getListen());
	if (bind(serverFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind");
        std::exit(EXIT_FAILURE);
    }
    if (listen(serverFd, 10) < 0) {
        perror("listen");
        std::exit(EXIT_FAILURE);
    }
}

size_t Server::getListen() {
    return (config.getListen());
}

int Server::getServerFd() {
    return (serverFd);
}

// bool Server::checkSocketError(int idx, ) {
//     if (events[idx].flags == EV_ERROR) {
//         if (events[idx].ident == server_fd) {
//             perror("Server socket error");
//             std::exit(EXIT_FAILURE);
//         } else {
//             perror("Client socket error");
//             close(events[idx].ident);
//             client.erase(events[idx].ident);
//             return true;
//         }
//     }
//     return false;
// }

void Server::runServer(int kq, std::vector<struct kevent> & changeList, struct kevent (&eventList)[10]) {
    struct timespec timeout;
    timeout.tv_sec = 5;
    timeout.tv_nsec = 0;
    int count = 0;
    while (true) {
        int nev = kevent(kq, &changeList[0], changeList.size(), eventList, 10, &timeout);
        if (nev < 0) {
            perror("kevent");
            std::exit(EXIT_FAILURE);
        }
        else if (nev == 0) {
            continue ;
        }
        changeList.clear();
        std::cout << "NUMBER OF EVENT : " << nev << std::endl;
        for (int i=0; i < nev; i++) {
            // if (check_socket_error(i))
                continue ; // error check
            
        }
    }
    
}
