/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 14:42:02 by haejeong          #+#    #+#             */
/*   Updated: 2024/06/24 14:45:43 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

Server::Server() : serverFd(0) {
	std::cout << "Server default constructor called" << std::endl;
}

Server::~Server() {
	std::cout << "Server destructor called" << std::endl;
}