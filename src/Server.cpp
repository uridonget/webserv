/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 14:42:02 by haejeong          #+#    #+#             */
/*   Updated: 2024/07/12 12:06:52 by sangyhan         ###   ########.fr       */
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

bool Server::findMatchingLocation(std::string & requestURL, Location & location) 
{
    std::vector<Location> locationList = config.getLocationList();
    bool res = false;
    std::vector<std::string> splitUrl = ft_split(requestURL, '/'); // 0
    std::vector<std::string> splitPath;
    for (std::vector<Location>::const_iterator it = locationList.begin(); it != locationList.end(); ++it) {
        const std::string &locPath = it->getPath();
        splitPath = ft_split(locPath, '/');
        int j = splitUrl.size();
        int k = splitPath.size();
        if (j == 0 && k == 0) {
            location = *it;
            return true;
        }
        int i = 0;
        while (i < j && i < k) {
            if (splitUrl[i] != splitPath[i])
                break;
            if (i + 1 == j || i + 1 == k) {
                if (j == k || j == k + 1) {
                    res = true;
                    location = *it;
                }
            }
            i++;
        }
    }
    return res;
}

int Server::afterProcessRequest(Buffer *file, struct kevent &change)
{  
    // std::cout << "file end!" << std::endl;
    std::map<Buffer *, std::pair<Buffer *, HttpRequest*> >::iterator it = requestList.find(file);
    if (it != requestList.end()) {
        if (it->second.second->fileCount == 1)
        {
            Buffer *res = it->second.first;
            std::string body(file->getReadBuffer().begin(), file->getReadBuffer().end());
            int code = 200;
            std::string message = "OK";
            std::string header = makeHeader(*(it->second.second), code, message, body);
            res->getWriteBuffer().insert(res->getWriteBuffer().end(), header.begin(), header.end());
            res->getWriteBuffer().insert(res->getWriteBuffer().end(), body.begin(), body.end());
            EV_SET(&change, res->getFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
            delete it->second.second;
            requestList.erase(file);
            std::cout << "response created!" << std::endl;
            return 1;
        }
        else
        {
            // std::cout << "file count" << it->second.second->fileCount << std::endl;
            it->second.second->fileCount -= 1;
            requestList.erase(file);
        }
    }
    return 0;
}

int Server::checkValid(HttpRequest & request, std::string & target) {
    if (request.method == NONE || request.url == "" || request.httpVersion == "") {
        return 400;
    }
    Location myLocation;
    bool isLocation = findMatchingLocation(request.url, myLocation);
    if (isLocation == false) { // there is no matching location
        std::cout << "LOCATION FALSE" << std::endl;
        if (request.method != NONE) {
            std::set<METHOD> allowedMethod = config.getAllowedMethods();
            if (allowedMethod.find(request.method) == allowedMethod.end()) {
                return 405;
            }
        }
        if (config.getRoot() == "")
            target += config.getRoot();    
        int isDir = isDirectory(request.url);
        if (isDir == 1) {
            return 404;
        } else if (isDir == 2) {
            // ******************** 아 몰라 나중에 수정 **************************
        }
    }
    else // there is matching location => use location block configuration
    {
        std::cout << "LOCATION TRUE" << std::endl;
        if (request.method != NONE) {
            std::set<METHOD> Allowed;
            if (myLocation.getAllowedMethods().size()) { // location에 allowedMethod가 있음
                Allowed = myLocation.getAllowedMethods();   
            } else if (config.getAllowedMethods().size()) { // location에 allowedMethod가 없고 server에는 있음
                Allowed = config.getAllowedMethods();
            }
            if (Allowed.find(request.method) == Allowed.end()) {
                return 405;
            }
        }
        if (myLocation.getRoot() != "") {
            target += myLocation.getRoot(); // location에 root가 있으면 location root를 넣고
        } else if (config.getRoot() != "") {
            target += config.getRoot(); // location에 root가 없으면 server root를 넣기
        } else {
            return 404;
        }
        std::cout << "TARGET : " << target << std::endl;
        // if (target[target.size() - 1] == '/') {
        //     target.erase(target.size() - 1);
        // }
        
        target += request.url.substr(myLocation.getPath().length(), request.url.length());
        std::cout << "TARGET : " << target << std::endl;
        int isDir = isDirectory(target);
        if (isDir == 1) {
            return 404;
        } else if (isDir == 3) {
            if (request.url == "/") {
                target += '/';
                if (myLocation.getIndex() != "") {
                    target += myLocation.getIndex();
                } else if (config.getIndex() != "") {
                    target += config.getIndex();
                } else {
                    return 404;
                }
            } else if (request.url == myLocation.getPath() + '/') {
                if (myLocation.getIndex() != "") {
                    target += myLocation.getIndex();
                } else if (config.getIndex() != "") {
                    target += config.getIndex();
                } else {
                    return 404;
                }
            }
        }
        std::cout << "TARGET : " << target << std::endl;
    }
    request.url = target;
    return 200;
}

static void pushEvent(int fd, int filter, int flag, std::vector <struct kevent> &changeList)
{
    struct kevent change;
    EV_SET(&change, fd, filter, flag, 0, 0, NULL);
    changeList.push_back(change);
}

std::vector<Buffer*> Server::processRequest(Buffer *client, HttpRequest &request, std::vector <struct kevent> &changeList)
{
    std::string target;
    std::vector<Buffer *> resList;
    int code = checkValid(request, target);
    if (code == 200) {
        if (request.method == GET && access(target.c_str(), F_OK | R_OK) == 0) {
            int fileFd = open(target.c_str(), O_RDONLY);
            if (fileFd > 0) // file open ok
            {
                File *res = new File(fileFd);
                resList.push_back(static_cast<Buffer *>(res));
                HttpRequest *request_share = new HttpRequest;
                *request_share = request;
                request_share->fileCount = 1;
                requestList[static_cast<Buffer *>(res)] = std::make_pair(client, request_share);
                pushEvent(fileFd, EVFILT_READ, EV_ADD | EV_ENABLE, changeList);
                return resList;
            }
        }
        else if (request.method == POST)
        {
            std::map<std::string, std::string>::iterator contentType = request.headers.find("Content-Type");
            std::string boundary = "";
            if (contentType != request.headers.end())
            {
                if (contentType->second.find("multipart/form-data;") != std::string::npos)
                {
                    size_t pos = contentType->second.find("boundary=");
                    if (pos != std::string::npos)
                    {
                        boundary = contentType->second.substr(pos + 9, std::string::npos);
                    }
                }    
            }
            if (boundary.size() != 0)
            {
                MimeParser parser(boundary, request.body);
                resList = parser.parse(config.getRoot());
                if (resList.size() > 0)
                {
                    HttpRequest *request_share = new HttpRequest;
                    *request_share = request;
                    request_share->fileCount = resList.size();
                    for(int i= 0 ; i < resList.size(); i++)
                    {
                        pushEvent(resList[i]->getFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, changeList);
                        requestList[static_cast<Buffer *>(resList[i])] = std::make_pair(client, request_share);
                    }
                    return resList;
                }
            }
            code = 400;
        }
        else
            code = 404;
    }
    if (code != 200) {
        std::cout << "code : " << code << std::endl;
        std::string response = makeResponseWithNoBody(request, code);
        client->getWriteBuffer().insert(client->getWriteBuffer().end(), response.begin(), response.end());
        pushEvent(client->getFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, changeList);
    }
    return resList;
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

std::string Server::makeResponseWithNoBody(HttpRequest &request, int code) {
	std::ostringstream response;
    std::string body;
    std::string header;
    std::string message;

    // if (request.method == GET) {
        if (code / 100 == 3) {
            message = "Moved Permanently";
            body = "";    
        } else if (code / 100 == 4) {
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
            body = makeErrorPage(code, message);
        }
        response << makeHeader(request, code, message, body);
        response << body;
    // }
    return response.str();
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
    if (code / 100 == 4 || code / 100 == 3)
        contentType = "text/html";
    else
        contentType = getContentType(request.url);
    oss << request.httpVersion << " " << code << " " << message << "\r\n";
    oss << "Content-Type: " << contentType << "\r\n";
    oss << "Content-Length: " << body.size() << "\r\n";
    oss << "Connection: keep-alive\r\n";
    if (code / 100 == 3) {
        oss << "Location: " << request.url << "\r\n";
    }
    oss << "\r\n";
    return oss.str();
}
