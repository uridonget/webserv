/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 10:49:19 by haejeong          #+#    #+#             */
/*   Updated: 2024/07/22 13:48:59 by haejeong         ###   ########.fr       */
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
#include "Library.hpp"

class Server;

class Webserv {
	private:
		Webserv(const Webserv & other);
		Webserv & operator=(const Webserv & other);
		
		ConfigParsing						configParsing;
		std::map<int, Server> 				serverList;
		std::map<int, int> 					serverFdMap;
		int 								kq;
		std::vector<struct kevent> 			changeList;
		struct kevent 						eventList[1000];
		std::vector<Buffer*> 				bufferList;

	public:
		Webserv();
		~Webserv();
		
		void 	configurationParsing(const std::string & configPath);
		void 	initKqueue();
		void 	makeServerList();
		void 	registerServerWithChangeList();

		bool 	checkSocketError(int idx);
		int		checkNewClient(unsigned long enventIdent);
		void	newClient(int serverFd);
		void	readEvent(int idx, int bufferIdx, int serverFd);
		void	writeEvent(int idx, int bufferIdx, int serverFd);
		void 	runServers();

		// websev utils
		int		isMessage(int bufferIdx);
		void	closeSocket(int bufferIdx);
		void	closeFile(int bufferIdx);
		void 	successResponse(int bufferIdx);
		void 	successFileWrite(int bufferIdx);
};

#endif