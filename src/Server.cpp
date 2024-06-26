/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 14:42:02 by haejeong          #+#    #+#             */
/*   Updated: 2024/06/25 17:12:21 by haejeong         ###   ########.fr       */
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
