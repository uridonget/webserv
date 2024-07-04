/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 14:42:02 by haejeong          #+#    #+#             */
/*   Updated: 2024/07/04 16:39:19 by sangyhan         ###   ########.fr       */
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

void Server::afterProcessRequest(Buffer *file, struct kevent &change)
{  
    std::map<Buffer *, std::pair<Buffer *, HttpRequest> >::iterator it = requestList.find(file);
    if (it != requestList.end())
    {
        std::string header = makeResponse(it->second.second, 200, static_cast<Buffer*>(file));
        Buffer *res = it->second.first;
        requestList.erase(file);
        res->getWriteBuffer().insert(res->getWriteBuffer().end(), header.begin(), header.end());
        res->getWriteBuffer().insert(res->getWriteBuffer().end(), file->getReadBuffer().begin(), file->getReadBuffer().end());
        EV_SET(&change, res->getFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
        return ;
    }
    return ;
}

Buffer *Server::processRequest(Buffer *client, HttpRequest &request, struct kevent &change)
{
    std::string target;
    target += config.getRoot();
    target += request.url;
    if (access(target.c_str(), F_OK | R_OK) == 0)
    {
        int fileFd = open(target.c_str(), O_RDONLY);
        if (fileFd > 0)
        {
            std::cout << target << std::endl;
            File *res = new File(fileFd);
            requestList[static_cast<Buffer *>(res)] = std::make_pair(client, request);
            EV_SET(&change, fileFd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
            return static_cast<Buffer *>(res);
        }
        else
        {
            std::string header = makeResponse(request, 404, 0);
            std::cout << header << std::endl;
            client->getWriteBuffer().insert(client->getWriteBuffer().end(), header.begin(), header.end());
            EV_SET(&change, client->getFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
            return 0;
        }
    }
    else
    {
        std::string header = makeResponse(request, 404, 0);
        std::cout << header << std::endl;
        client->getWriteBuffer().insert(client->getWriteBuffer().end(), header.begin(), header.end());
        EV_SET(&change, client->getFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
        return 0;
    }
}

static std::string getContentType(std::string url)
{
    size_t pos = url.find_last_of(".", std::string::npos);
    std::string res;
    if (pos == std::string::npos)
    {
        res = "text/plain";
    }
    else
    {
        std::string filetype = url.substr(pos + 1, std::string::npos);
        if (filetype == "js" || filetype == "css" || filetype == "html" || filetype == "javascript" || filetype == "csv"
            || filetype == "xml")
        {
            res += "text/";
            res += filetype;
        }
        else if (filetype == "jpeg" || filetype == "jpg" || filetype == "png" || filetype == "gif" || filetype == "tiff")
        {
            res += "image/";
            res += filetype;
        }
        else if (filetype == "mpeg") // || filetype == "x-ms-wma" || filetype == "vnd.rn-realaudio" || filetype == "x-wav")
        {
            res += "audio/";
            res += filetype;
        }
        else if (filetype == "mpeg" || filetype == "mp4" || filetype == "webm" || filetype == "mov")
        {
            res += "vedio/";
            res += filetype;
        }
    }
    return res;
}

std::string Server::makeResponse(HttpRequest &request, int code, Buffer *buffer) {
	std::ostringstream response;

    if (request.method == "GET") 
    {
        if (code == 200)
        {
            response << request.httpVersion << " 200 OK\r\n";
        }
        else
        {
            response << request.httpVersion << " 404 Not found\r\n";
        }
        response << "Content-Type: " << getContentType(request.url) << "\r\n";
        response << "Connection: close\r\n";
        if (buffer != 0)
        {
            response << "Content-Length: " << std::to_string(buffer->getReadBuffer().size()) << "\r\n";
        }
        else
        {
            response << "Content-Length: 0" << "\r\n";
        }
        response << "\r\n";
    } else {
        std::string body = "<html><body><h1>403 Not Found</h1></body></html>";
        response << request.httpVersion << " 404 NOTFound\r\n";
        response << "Content-Type: text/html\r\n";
        response << "Content-Length: " << std::to_string(body.size()) << "\r\n";
        response << "Connection: close\r\n";
        response << "\r\n";
        response << body;
    }
    std::string responseStr = response.str();
    return responseStr;
}
