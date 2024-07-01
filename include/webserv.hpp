/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 10:49:19 by haejeong          #+#    #+#             */
/*   Updated: 2024/06/28 18:44:22 by haejeong         ###   ########.fr       */
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
#include "RequestParser.hpp"

class Server;

class Webserv {
	private:
		ConfigParsing						configParsing;
		std::map<int, Server> 				serverList; // <port, Server>
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

		bool checkSocketError(int idx, int bufferIdx);
		void runServers();
		
		int checkNewClient(uintptr_t enventIdent);

		void newClient(int serverFd);
		std::string makeResponse();
		void readEvent(int idx, int bufferIdx);
		void writeEvent(int idx, int bufferIdx);

		// websev utils
		int isMessage(int bufferIdx);
		void closeSocket(int bufferIdx);
		void closeFile(int bufferIdx);
		void successResponse(int bufferIdx);
		void successFileWrite(int bufferIdx);

};

void setNonblock(int fd);

#endif