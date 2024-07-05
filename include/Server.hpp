/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 16:22:49 by haejeong          #+#    #+#             */
/*   Updated: 2024/07/05 13:00:10 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "ServerConfig.hpp"
#include "Webserv.hpp"
#include "Buffer.hpp"
#include "File.hpp"
#include "Message.hpp"
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

class Server {
	private:
		ServerConfig 										config;
		int													serverFd;
		struct sockaddr_in 									serverAddr;
		std::map<Buffer*, std::pair<Buffer*, HttpRequest> >	requestList;
		
	public:
		Server();
		~Server();

		ServerConfig getConfig();
		int getServerFd() const;
		void afterProcessRequest(Buffer *file, struct kevent &change); // file을 열 수 있는지 확인하고 존재하면 buffer(파일)을 생성한다.
		Buffer *processRequest(Buffer *client, HttpRequest &request, struct kevent &change); 
		std::string makeResponse(HttpRequest &request, int code, Buffer *buffer);
		void initServer(ServerConfig & config);
		// bool findMatchingLocation(std::string & requestURL, Location & location);
		void HttpRequestValidCheck(HttpRequest & request, int & code, std::string & message);
		std::string makeErrorPage(int & code, std::string & message);
		std::string makeBody(HttpRequest & request, int & code, std::string & message);
		std::string makeHeader(HttpRequest & request, int & code, std::string & message, std::string & body);
		// void makeResponse(HttpRequest & request, Buffer * buffer);
		// 파싱된 request 구조체 넣어주면 그걸로 response 만들어서 buffer에다가 넣기

};

#endif