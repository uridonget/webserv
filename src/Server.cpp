/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 14:42:02 by haejeong          #+#    #+#             */
/*   Updated: 2024/07/03 18:05:29 by sangyhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Library.hpp"

Server::Server() : serverFd(0) {
	// std::cout << "Server default constructor called" << std::endl;
}

Server::~Server() {
	// std::cout << "Server destructor called" << std::endl;
}

void Server::initServer(ServerConfig & config) {
	this->config = config;
	serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (serverFd < 0) {
		throw RuntimeException("socket");
	}

	setNonblock(serverFd);
		
	int opt = 1;
	if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		throw RuntimeException("setsockopt");
	}
		
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(this->getListen());

	std::cout << "PORT : " << this->getListen() << std::endl;
		
	if (bind(serverFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
		throw RuntimeException("bind");
	}
		
	if (listen(serverFd, 10) < 0) {
		throw RuntimeException("listen");
	}
}

size_t Server::getListen() {
	return (config.getListen());
}

int Server::getServerFd() {
	return (serverFd);
}

Message *Server::afterProcessRequest(File &file)
{  
    std::map<File *, HttpRequest>::iterator it = requestList.find(&file);
    if (it != requestList.end())
    {
        std::string header = makeResponse(it->second, static_cast<Buffer*>(&file));
        Message *res = new Message(it->second.fd);
        res->getWriteBuffer().insert(res->getWriteBuffer().begin(), header.begin(), header.end());
        res->getWriteBuffer().insert(res->getWriteBuffer().begin(), file.getReadBuffer().begin(), file.getReadBuffer().end());
        return res;
    }
    return (0);
}

File *Server::processRequest(HttpRequest &request)
{
    // if (request.method == GET)
    // {
    std::string target;
    target += config.getRoot();
    target += request.url;
    int fileFd = open(target.c_str(), O_RDONLY);
    if (fileFd > 0)
    {
        // File *res = new File;
    }
    else
    {
        // makeResponse(request,)
    }
    // }
}

std::string Server::makeResponse(HttpRequest &request, Buffer *buffer) {
	std::ostringstream response;
    
    std::cout << "address : " << &buffer << std::endl;

    if (request.method == "GET") {
        // 기본 상태 라인
        response << request.httpVersion << " 200 OK\r\n";
        
        // 기본 헤더
        response << "Content-Type: text/html\r\n";
        response << "Connection: close\r\n";

        // Content-Length 헤더 추가
        if (buffer != 0)
            response << "Content-Length: " << buffer->getReadBuffer().size() << "\r\n";
        // 빈 줄로 헤더와 본문 구분
        response << "\r\n";
        
    } else {
        // GET 메소드가 아닌 경우 405 Method Not Allowed 응답
        std::string body = "<html><body><h1>405 Method Not Allowed</h1></body></html>";
        response << request.httpVersion << " 405 Method Not Allowed\r\n";
        response << "Content-Type: text/html\r\n";
        response << "Content-Length: " << body.size() << "\r\n";
        response << "Connection: close\r\n";
        response << "\r\n";
        response << body;
    }
    std::string responseStr = response.str();
    return responseStr;
}
