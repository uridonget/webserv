/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 16:22:49 by haejeong          #+#    #+#             */
/*   Updated: 2024/07/09 15:31:56 by haejeong         ###   ########.fr       */
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
		
		void parseQueryString(HttpRequest & request, const std::string& query);

		void urlParsing(HttpRequest & request);

		int checkValid(HttpRequest & request, std::string & target);

		Buffer *processRequest(Buffer *client, HttpRequest &request, struct kevent &change); 

		void afterProcessRequest(Buffer *file, struct kevent &change); // file을 열 수 있는지 확인하고 존재하면 buffer(파일)을 생성한다.
		
		std::string makeResponse(HttpRequest &request, int code, Buffer *buffer);

		void initServer(ServerConfig & config);
		
		bool findMatchingLocation(std::string & requestURL, Location & location);
		
		void HttpRequestValidCheck(HttpRequest & request, int & code, std::string & message);
		
		std::string makeErrorPage(int & code, std::string & message);
		
		std::string makeBody(HttpRequest & request, int & code, std::string & message);
		
		std::string makeHeader(HttpRequest & request, int & code, std::string & message, std::string & body);
		

};

#endif