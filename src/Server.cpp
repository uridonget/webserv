/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 14:42:02 by haejeong          #+#    #+#             */
/*   Updated: 2024/07/05 19:43:29 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Library.hpp"

Server::Server() : serverFd(0) {}

Server::~Server() {}

ServerConfig Server::getConfig() {
    return config;
}

int Server::getServerFd() const{
	return serverFd;
}

void Server::initServer(ServerConfig & config) {
	this->config = config;
	serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (serverFd < 0) {
		throw RuntimeException("socket");
	}
	setNonblock(serverFd);	
	int opt = 1;
	if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
		throw RuntimeException("setsockopt");
	}
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(config.getListen());
	if (bind(serverFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
		throw RuntimeException("bind");
	}	
	if (listen(serverFd, 10) < 0) {
		throw RuntimeException("listen");
	}
}

void Server::afterProcessRequest(Buffer *file, struct kevent &change)
{  
    std::map<Buffer *, std::pair<Buffer *, HttpRequest> >::iterator it = requestList.find(file);
    if (it != requestList.end()) {
        Buffer *res = it->second.first;
        std::string body(file->getReadBuffer().begin(), file->getReadBuffer().end());
        int code = 200;
        std::string message = "OK";
        std::string header = makeHeader(it->second.second, code, message, body);
        res->getWriteBuffer().insert(res->getWriteBuffer().end(), header.begin(), header.end());
        res->getWriteBuffer().insert(res->getWriteBuffer().end(), body.begin(), body.end());
        requestList.erase(file);
        EV_SET(&change, res->getFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
    }
}

int Server::checkValid(HttpRequest & request, std::string & target) {
    if (request.method == NONE || request.url == "" || request.httpVersion == "") {
        return 400;
    }
    std::vector<Location> locationList = config.getLocationList();    
    std::vector<Location>::iterator it = locationList.begin();
    size_t len = 0;
    Location location;
    for (; it != locationList.end(); it++) {
        if (request.url.compare(0, (*it).getPath().length(), (*it).getPath()) == 0) {
            if (len < (*it).getPath().length()) {
                len = (*it).getPath().length();
                location = (*it);
            }
        }
    }
    if (len == 0) { // there is no matching location => use server block configuration
        if (request.method != NONE) {
            std::set<METHOD> allowedMethod = config.getAllowedMethods();
            if (allowedMethod.find(request.method) == allowedMethod.end()) {
                return 405;
            }
        }
        target += config.getRoot();
        target += config.getIndex();
    }
    else // there is matching location => use location block configuration
    {
        if (request.method != NONE) {
            std::set<METHOD> Allowed;
            if (location.getAllowedMethods().size()) { // location에 allowedMethod가 있음
                Allowed = location.getAllowedMethods();   
            } else if (config.getAllowedMethods().size()) { // location에 allowedMethod가 없고 server에는 있음
                Allowed = config.getAllowedMethods();
            }
            if (Allowed.find(request.method) == Allowed.end()) {
                return 405;
            }
        }
        if (location.getRoot() != "") {
            target += location.getRoot(); // location에 root가 있으면 location root를 넣고
        } else if (config.getRoot() != "") {
            target += config.getRoot(); // location에 root가 없으면 server root를 넣기
        } else {
            return 404;
        }
        if (request.url == location.getPath()) { // request url과 location path가 동일할 때
            if (location.getIndex() != "") { //location에 index가 있으면 해당 index를 넣고
                target += location.getIndex();
            } else if (config.getIndex() != "") { // location에 index가 없으면 서버 index를 넣기
                target += config.getIndex();
            } else {
                return 404;
            }
        } else { // request url과 location path가 동일하지 않을 때 (따로 특정한 파일을 요구할 때)
            target += request.url;
        }
    }
    request.url = target;
    std::cout << "\nTARGET : " << target << std::endl;
    return 200;
}

Buffer* Server::processRequest(Buffer *client, HttpRequest &request, struct kevent &change)
{
    std::string target;
    int code = checkValid(request, target);
    if (code == 200) {
        if (access(target.c_str(), F_OK | R_OK) == 0) {
            int fileFd = open(target.c_str(), O_RDONLY);
            if (fileFd > 0) // file open ok
            {
                File *res = new File(fileFd);
                requestList[static_cast<Buffer *>(res)] = std::make_pair(client, request);
                EV_SET(&change, fileFd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                return static_cast<Buffer *>(res);
            }
        }
        code = 404;
    }
    if (code != 200) {
        std::string message;
        if (code == 400) {
            message = ERR400;
        } else if (code == 403) {
            message = ERR403;
        } else if (code == 404) {
            message = ERR404;
        } else if (code == 405) {
            message = ERR405;
        } else if (code == 408) {
            message = ERR408;
        } else if (code == 411) {
            message = ERR411;
        } else if (code == 413) {
            message = ERR413;
        } else if (code == 415) {
            message = ERR415;
        }
        std::string body = makeErrorPage(code, message);
        std::string response = makeHeader(request, code, message, body);
        response += body;
        client->getWriteBuffer().insert(client->getWriteBuffer().end(), response.begin(), response.end());
        EV_SET(&change, client->getFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
        return 0;
    }
    return 0;
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

    if (request.method == GET) 
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
    return response.str();
}

std::string Server::makeErrorPage(int & code, std::string & message) {
    std::ostringstream oss;

    oss << "<!DOCTYPE html>\n";
    oss << "<html lang=\"en\">\n";
    oss << "<head>\n";
    oss << "<meta charset=\"UTF-8\">\n";
    oss << "<title>Error " << code << "</title>\n";
    oss << "<style>\n";
    oss << "body { font-family: Arial, sans-serif; background-color: #f8f9fa; color: #343a40; text-align: center; margin-top: 50px; }\n";
    oss << "h1 { font-size: 3em; color: #d9534f; }\n";
    oss << "p { font-size: 1.2em; color: #6c757d; }\n";
    oss << "a { color: #007bff; text-decoration: none; }\n";
    oss << "a:hover { text-decoration: underline; }\n";
    oss << ".container { max-width: 600px; margin: 0 auto; }\n";
    oss << "</style>\n";
    oss << "</head>\n";
    oss << "<body>\n";
    oss << "<div class=\"container\">\n";
    oss << "<h1>Error " << code << "</h1>\n";
    oss << "<p>" << message << "</p>\n";
    oss << "</div>\n";
    oss << "</body>\n";
    oss << "</html>";

    return oss.str();
}

// std::string readFromPipe(int pipeFd) {
//     const size_t bufferSize = 1000;
//     std::vector<char> buffer(bufferSize);
//     std::string result;

//     while (true) {
//         ssize_t bytesRead = read(pipeFd, buffer.data(), bufferSize);
//         if (bytesRead < 0) {
//             break;
//         }
//         if (bytesRead == 0) {
//             break;
//         }
//         result.append(buffer.data(), bytesRead);
//     }
//     result.push_back(0);
//     return (result);
// }

// std::string Server::makeBody(HttpRequest & request, int & code, std::string & message) {
//     if (code != 200) {
//         return (makeErrorPage(code, message));
//     }
//     // 여기에서 이미지를 요청하면 이미지를 보낼 수 있도록 body를 만들기
//     std::string body;
//     int pipefd[2];
//     int pipeOut[2];
//     if (pipe(pipefd) < 0) {
//         throw RuntimeException("pipe");
//     }
//     if (pipe(pipeOut) < 0) {
//         throw RuntimeException("pipe");
//     }
//     pid_t pid = fork();
//     if (pid < 0) {
//         throw RuntimeException("fork");
//     }
//     if (pid == 0) { // 자식 프로세스
//         if (dup2(pipefd[0], STDIN_FILENO) < 0) {
//             throw RuntimeException("dup2");
//         }
//         if (dup2(pipeOut[1], STDOUT_FILENO) == -1) {
//             throw RuntimeException("dup2");
//         }
//         close(pipefd[0]);
//         close(pipefd[1]);
//         close(pipeOut[0]);
//         close(pipeOut[1]);

//         std::string fileName = "cgi-bin/hello.py";
//         std::vector<char *> exec_args;
//         exec_args.push_back(const_cast<char *>(fileName.c_str()));
//         exec_args.push_back(NULL);
//         char *envp[1];
//         envp[0] = 0;
//         execve(fileName.c_str(), exec_args.data(), envp);
//         perror("execve");
//         throw RuntimeException("execve");
//     } else {
//         int status;
//         std::vector<char> buffer(BUFFER_SIZE);
//         std::string result;

//         close(pipefd[0]);
//         close(pipefd[1]);
//         close(pipeOut[1]);
//         waitpid(pid, &status, 0);
//         if (WIFEXITED(status)) {
//             std::string result = readFromPipe(pipeOut[0]);
//             for (int i=0; i < result.size(); i++) {
//                 body.push_back(result[i]);
//             }
//         }
//         return (body);
//     }
// }

std::string Server::makeHeader(HttpRequest & request, int & code, std::string & message, std::string & body) {
    std::ostringstream oss;
    std::string contentType;
    if (code / 100 == 4)
        contentType = "text/html";
    else
        contentType = getContentType(request.url);
    oss << request.httpVersion << " " << code << " " << message << "\r\n";
    oss << "Content-Type: " << contentType << "\r\n";
    oss << "Content-Length: " << body.size() << "\r\n";
    oss << "Connection: keep-alive\r\n";
    oss << "\r\n";
    return oss.str();
}
