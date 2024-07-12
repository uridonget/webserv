/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 16:22:49 by haejeong          #+#    #+#             */
/*   Updated: 2024/07/11 21:25:04 by sangyhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include "ServerConfig.hpp"
#include "Webserv.hpp"
#include "Buffer.hpp"
#include "File.hpp"
#include "Message.hpp"
#include "MimeParser.hpp"

class Server {
	private:
		ServerConfig 										config;
		int													serverFd;
		struct sockaddr_in 									serverAddr;
		std::map<Buffer*, std::pair<Buffer*, HttpRequest*> >	requestList;
		
	public:
		Server();
		~Server();

		ServerConfig getConfig();
		
		int getServerFd() const;
		
		void parseQueryString(HttpRequest & request, const std::string& query);

		void urlParsing(HttpRequest & request);

		int checkValid(HttpRequest & request, std::string & target);

		std::vector<Buffer*> processRequest(Buffer *client, HttpRequest &request, std::vector <struct kevent> &changeList);

		int afterProcessRequest(Buffer *file, struct kevent &change); // file을 열 수 있는지 확인하고 존재하면 buffer(파일)을 생성한다.
		
		std::string makeResponseWithNoBody(HttpRequest &request, int code);

		void initServer(ServerConfig & config);
		
		bool findMatchingLocation(std::string & requestURL, Location & location);
		
		void HttpRequestValidCheck(HttpRequest & request, int & code, std::string & message);
		
		std::string makeErrorPage(int & code, std::string & message);
		
		std::string makeHeader(HttpRequest & request, int & code, std::string & message, std::string & body);
		
		void handleCGI(int tmpFileFd, std::string & target, HttpRequest &request);

};

#endif