/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 14:31:23 by haejeong          #+#    #+#             */
/*   Updated: 2024/06/24 14:45:41 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ServerConfig.hpp"

ServerConfig::ServerConfig()
    : listen(0),
      serverName(""),
      root("/"),
      clientMaxBodySize(0),
      index("index.html"),
      redirection("") {
    std::cout << "ServerConfig default constructor called" << std::endl;
}

ServerConfig::~ServerConfig() {
	std::cout << "ServerConfig destructor called" << std::endl;
}

