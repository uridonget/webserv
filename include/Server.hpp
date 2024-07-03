/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 16:22:49 by haejeong          #+#    #+#             */
/*   Updated: 2024/07/03 18:26:26 by sangyhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "ServerConfig.hpp"
#include "Webserv.hpp"
#include "Buffer.hpp"
#include "File.hpp"
#include "Message.hpp"

class Server {
	private:
		ServerConfig 					config;
		int								serverFd;
		struct sockaddr_in 				serverAddr;
		std::map<File*, HttpRequest>	requestList;
		
	public:
		Server();
		~Server();

		void initServer(ServerConfig & config);
		size_t getListen();
		int getServerFd();

		Message *afterProcessRequest(File &file);
		File *processRequest(HttpRequest &request); 
		std::string makeResponse(HttpRequest &request, Buffer *buffer);
		// 파싱된 request 구조체 넣어주면 그걸로 response 만들어서 buffer에다가 넣기

};

#endif