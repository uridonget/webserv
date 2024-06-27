/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 16:22:49 by haejeong          #+#    #+#             */
/*   Updated: 2024/06/27 13:40:21 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "ServerConfig.hpp"

class Server {
	private:
		ServerConfig 					config;
		int								serverFd;
		struct sockaddr_in 				serverAddr;

		
	public:
		Server();
		~Server();

		void initServer(ServerConfig & config);
		size_t getListen();
		int getServerFd();

		void set_nonblock(int fd);
};

#endif