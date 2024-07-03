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
	if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
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

ServerConfig Server::getConfig() {
    return this->config;
}

size_t Server::getListen() {
	return config.getListen();
}

int Server::getServerFd() const{
	return this->serverFd;
}

std::string readFromPipe(int pipeFd) {
    const size_t bufferSize = 1000;
    std::vector<char> buffer(bufferSize);
    std::string result;

    while (true) {
        ssize_t bytesRead = read(pipeFd, buffer.data(), bufferSize);
        if (bytesRead < 0) {
            perror("read");
            break;
        }
        if (bytesRead == 0) {
            break;
        }
        result.append(buffer.data(), bytesRead);
    }
    result.push_back(0);
    return (result);
}

bool Server::findMatchingLocation(std::string & requestURL, Location & location) {
    std::vector<Location> locationList = config.getLocationList();
    bool res = false;
    for (std::vector<Location>::const_iterator it = locationList.begin(); it != locationList.end(); ++it) {
        const std::string &locPath = it->getPath();
        if (requestURL.compare(0, locPath.length(), locPath) == 0) {
            if (locPath.length() <= location.getPath().length()) {
                location = *it;
                res = true;
                std::cout << "MATCH!" << std::endl;
                std::cout << "url  : " << requestURL << std::endl;
                std::cout << "path : " << locPath << std::endl;
            }
        }
    }
    return res;
}

void Server::HttpRequestValidCheck(HttpRequest & request, int & code, std::string & message) {
    if (request.method == NONE) {
        code = 400;
        message = "Bad Request: HTTP method is missing.";
        return;
    }
    if (request.url.empty()) {
        code = 400;
        message = "Bad Request: URL is missing.";
        return;
    }
    // 여기에 들어가야만 해...
    // 1. 해당하는 location이 있기는 한지
    // 2. 있다면 레퍼런스로 받아서 값을 담아줘
    Location location;
    bool matchLocation = findMatchingLocation(request.url, location);
    if (matchLocation) {
        std::cout << "LOCATION SIDE\n";
        std::set<METHOD> allowedMethod = location.getAllowedMethods();
        for (std::set<METHOD>::iterator it = allowedMethod.begin(); it != allowedMethod.end(); it++) {
            std::cout << "CHECK!! : " << *it << std::endl;
        }
        if (allowedMethod.size() && allowedMethod.find(request.method) == allowedMethod.end()) {
            code = 405;
            message = "Method Not Allowed: Invalid HTTP method.";
            return;    
        }
    } else {
        std::set<METHOD> allowedMethod = location.getAllowedMethods();
        std::cout << "SERVER SIDE\n";
        for (std::set<METHOD>::iterator it = allowedMethod.begin(); it != allowedMethod.end(); it++) {
            std::cout << "CHECK!! : " << *it << std::endl;
        }
        if (allowedMethod.size() && allowedMethod.find(request.method) == allowedMethod.end()) {
            code = 405;
            message = "Method Not Allowed: Invalid HTTP method.";
            return; 
        }
    }
    if (request.httpVersion != "HTTP/1.1") {
        code = 400;
        message = "Bad Request: Invalid HTTP version.";
        return;
    }
    if (request.host.empty()) {
        code = 400;
        message = "Bad Request: Host header is missing.";
        return;
    }
    if (request.method == POST) {
        if (request.headers.find("Content-Length") == request.headers.end()) {
            code = 411;
            message = "Length Required: Content-Length header is missing.";
            return;
        } else {
            try {
                int contentLength = std::stoi(request.headers["Content-Length"]);
                if (contentLength != request.body.size()) {
                    code = 400;
                    message = "Bad Request: Content-Length does not match the body size.";
                    return;
                }
            } catch (std::exception &e) {
                code = 400;
                message = "Bad Request: Invalid Content-Length value.";
                return;
            }
        }
    }
    code = 200;
    message = "OK";
}

std::string Server::makeErrorPage(int & code, std::string & message) {
    std::ostringstream oss;

    oss << "<html lang=\"en\">\n";
    oss << "<head>\n";
    oss << "<title>Error " << code << "</title>\n";
    oss << "<style>\n";
    oss << "body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }\n";
    oss << "h1 { font-size: 2em; color: #d9534f; }\n";
    oss << "p { font-size: 1em; color: #333; }\n";
    oss << "</style>\n";
    oss << "</head>\n";
    oss << "<body>\n";
    oss << "<h1>Error " << code << "</h1>\n";
    oss << "<p>" << message << "</p>\n";
    oss << "</body>\n";
    oss << "</html>";

    return oss.str();
}

std::string Server::makeBody(HttpRequest & request, int & code, std::string & message) {
    if (code != 200) {
        return (makeErrorPage(code, message));
    }
    // 여기에서 이미지를 요청하면 이미지를 보낼 수 있도록 body를 만들기
    std::string body;
    int pipefd[2];
    int pipeOut[2];
    if (pipe(pipefd) < 0) {
        throw RuntimeException("pipe");
    }
    if (pipe(pipeOut) < 0) {
        throw RuntimeException("pipe");
    }
    pid_t pid = fork();
    if (pid < 0) {
        throw RuntimeException("fork");
    }
    if (pid == 0) { // 자식 프로세스
        if (dup2(pipefd[0], STDIN_FILENO) < 0) {
            throw RuntimeException("dup2");
        }
        if (dup2(pipeOut[1], STDOUT_FILENO) == -1) {
            throw RuntimeException("dup2");
        }
        close(pipefd[0]);
        close(pipefd[1]);
        close(pipeOut[0]);
        close(pipeOut[1]);

        std::string fileName = "cgi-bin/hello.py";
        std::vector<char *> exec_args;
        exec_args.push_back(const_cast<char *>(fileName.c_str()));
        exec_args.push_back(NULL);
        char *envp[1];
        envp[0] = 0;
        execve(fileName.c_str(), exec_args.data(), envp);
        throw RuntimeException("execve");
    } else {
        int status;
        std::vector<char> buffer(BUFFER_SIZE);
        std::string result;

        close(pipefd[0]);
        close(pipefd[1]);
        close(pipeOut[1]);
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            std::string result = readFromPipe(pipeOut[0]);
            for (int i=0; i < result.size(); i++) {
                body.push_back(result[i]);
            }
        }
        return (body);
    }
}

std::string Server::makeHeader(HttpRequest & request, int & code, std::string & message, std::string & body) {
    std::ostringstream oss;
    // 위의 결과에 따라서 (Body 생성) Header도 적응형으로 생성하기
    oss << request.httpVersion << " " << code << " " << message << "\r\n";
    oss << "Content-Type: text/html\r\n";
    oss << "Content-Length: " << body.size() << "\r\n";
    oss << "Connection: keep-alive\r\n";
    oss << "\r\n";
    return oss.str();
}

void Server::makeResponse(HttpRequest & request, Buffer * buffer) {
    std::ostringstream response;
    int code;
    std::string message;
    
    HttpRequestValidCheck(request, code, message);
    std::string body = makeBody(request, code, message);
    response << makeHeader(request, code, message, body);
    response << body;
    std::string responseStr = response.str();
    // std::cout << "\n<RESPONSE>\n" << responseStr << std::endl;
	buffer->getWriteBuffer().insert(buffer->getWriteBuffer().end(), responseStr.begin(), responseStr.end());
}
