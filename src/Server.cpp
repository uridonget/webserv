/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 14:42:02 by haejeong          #+#    #+#             */
/*   Updated: 2024/07/01 17:09:09 by haejeong         ###   ########.fr       */
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

void Server::makeResponse(HttpRequest & request, Buffer * buffer) {
	std::ostringstream response;
    
    std::cout << "address : " << &buffer << std::endl;

    if (request.method == "GET") {
        // 기본 상태 라인
        response << request.httpVersion << " 200 OK\r\n";
        
        // 기본 헤더
        response << "Content-Type: text/html\r\n";
        response << "Connection: close\r\n";
        
        // 요청 URL에 따른 본문 내용 결정
        std::string body;
        if (request.url == "/") {
            body = "<html><body><h1>Welcome to the Home Page</h1></body></html>";
        } else {
            body = "<html><body><h1>404 Not Found</h1></body></html>";
        }

        // Content-Length 헤더 추가
        response << "Content-Length: " << body.size() << "\r\n";
        
        // 빈 줄로 헤더와 본문 구분
        response << "\r\n";
        
        // 응답 본문 추가
        response << body;
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
	// std::cout << "check response\n\n\n" << responseStr << std::endl;
	buffer->getWriteBuffer().insert(buffer->getWriteBuffer().end(), responseStr.begin(), responseStr.end());
    std::string check(buffer->getWriteBuffer().begin(), buffer->getWriteBuffer().end());
    // std::cout << "check!!!!!!!!!!!!!!!!!!!!\n\n\n" << check << std::endl;
}
