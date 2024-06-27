/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 10:49:19 by haejeong          #+#    #+#             */
/*   Updated: 2024/06/27 12:47:53 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include "Server.hpp"
#include "ConfigParsing.hpp"
#include "ServerConfig.hpp"
#include "Buffer.hpp"
#include "Message.hpp"
#include "File.hpp"

class Webserv {
	private:
		ConfigParsing						configParsing;
		std::map<int, Server> 				serverList; // <port, Server>
		// std::map<int, std::vector<char> > 	clients; // I/OMultiplexing으로 변경할 예정
		std::set<int> serverFdSet;
		int kq;
		std::vector<struct kevent> changeList;
		struct kevent eventList[10];
		std::vector<Buffer> bufferList;


	public:
		Webserv();
		~Webserv();

		void makeServerConfigStringList(const std::string & configPath);
		std::vector<ServerConfig> getServerConfigs();
		void makeServerList();
		void initKqueue();
		void connectKqueueToServer();
		void ioMultiplexing();
		bool checkSocketError(int idx);
		void runServers();
		
		int checkNewClient(uintptr_t enventIdent);

		void new_client(int serverFd);
		std::string make_response();
		void read_event(int idx);
		void write_event(int idx);
};

#endif