/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 16:22:49 by haejeong          #+#    #+#             */
/*   Updated: 2024/07/29 14:03:33 by sangyhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include "ServerConfig.hpp"
#include "Webserv.hpp"
#include "File.hpp"
#include "Message.hpp"
#include "Pipe.hpp"
#include "MimeParser.hpp"
#include "CgiParser.hpp"

class Server {
	private:
		ServerConfig 										config;
		int													serverFd;
		struct sockaddr_in 									serverAddr;
		std::map<Buffer*, std::pair<Buffer*, HttpRequest*> >	requestList;
		std::map<pid_t, clock_t>				childTime;
		
	public:
		Server();
		~Server();

		ServerConfig getConfig();
		int getServerFd() const;
		std::map<pid_t, clock_t>& getChildTime();
	
		int checkValid(HttpRequest & request, std::string & target);
		std::vector<Buffer*> processRequest(Buffer *client, HttpRequest &request, std::vector <struct kevent> &changeList);
		int afterProcessRequest(Buffer *file, struct kevent &change);
		std::vector<char> makeResponseWithNoBody(HttpRequest &request, int code);
		void initServer(ServerConfig & config);
		bool findMatchingLocation(std::string & requestURL, Location & location);
		bool findMatchingExtension(std::string & target, Location & myExtension);
		std::string makeDefaultErrorPage(int & code, std::string & message);
		std::string makeHeader(HttpRequest & request, int & code, std::string & message, std::vector<char> &body, std::string & contentType);
		pid_t handleCGI(int inputFileFd[2], int outputFileFd[2], std::string & target, HttpRequest &request);
		void deleteRequestByFile(Buffer *buf);
		char** makeEnvp(std::string & target, HttpRequest &request);
		void cgiHeaderParsing(std::string & cgiHeader, int & code, std::string &message, std::string & contentType);
};

#endif