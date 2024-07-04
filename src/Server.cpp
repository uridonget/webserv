/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 14:42:02 by haejeong          #+#    #+#             */
/*   Updated: 2024/07/04 18:48:56 by haejeong         ###   ########.fr       */
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

///////////////////////

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
            std::cout << "FILE EXIST!" << target << std::endl;
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

std::string readFromPipe(int pipeFd) {
    const size_t bufferSize = 1000;
    std::vector<char> buffer(bufferSize);
    std::string result;

    while (true) {
        ssize_t bytesRead = read(pipeFd, buffer.data(), bufferSize);
        if (bytesRead < 0) {
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
    std::set<METHOD> allowedMethod = config.getAllowedMethods();
    if (allowedMethod.size() && allowedMethod.find(request.method) == allowedMethod.end()) {
        code = 405;
        message = "Method Not Allowed: Invalid HTTP method.";
        return; 
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
        perror("execve");
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
    std::cout << "\n<RESPONSE>\n" << responseStr << std::endl;
	buffer->getWriteBuffer().insert(buffer->getWriteBuffer().end(), responseStr.begin(), responseStr.end());
}
