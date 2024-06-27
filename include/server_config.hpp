/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_config.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 16:23:48 by haejeong          #+#    #+#             */
/*   Updated: 2024/06/27 17:02:32 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_CONFIG_HPP
# define SERVER_CONFIG_HPP

#include "includes.hpp"

class ServerConfig {
	private:
		size_t							port;
		in_addr_t						host;
		std::string						serverName;
		unsigned long					clientMaxBodySize;
		std::map<int, std::string>		errorPages;
        struct sockaddr_in 				serverAddress;
        int     						serverFd;  // socket(AF_INET, SOCK_STREAM, 0)
		std::string						body;
	public:
		
};

#endif