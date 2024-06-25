/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 14:42:02 by haejeong          #+#    #+#             */
/*   Updated: 2024/06/25 15:09:26 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

Server::Server() : serverFd(0) {
	std::cout << "Server default constructor called" << std::endl;
}

Server::~Server() {
	std::cout << "Server destructor called" << std::endl;
}



void Server::makeConfig(std::string & configString) {
	std::cout << "CHECK\n" << configString << std::endl;
	
}