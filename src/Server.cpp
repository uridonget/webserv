/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 14:42:02 by haejeong          #+#    #+#             */
/*   Updated: 2024/08/20 12:49:06 by sangyhan         ###   ########.fr       */
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

std::map<pid_t, clock_t>& Server::getChildTime(){
	return childTime;
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
	if (listen(serverFd, 10000) < 0) {
		throw RuntimeException("listen");
	}
}

bool Server::findMatchingLocation(std::string & requestURL, Location & location) {
    std::vector<Location> locationList = config.getLocationList();
    std::vector<std::string> splitUrl = ft_split(requestURL, '/');
    int urlNum = splitUrl.size();
    std::vector<std::string> splitPath;
    bool ans = false;
    int deepest = -1;
    for (std::vector<Location>::const_iterator it = locationList.begin(); it != locationList.end(); ++it) {
        std::string locPath = it->getPath();
        splitPath = ft_split(locPath, '/');
        int pathNum = splitPath.size();
        if (pathNum == 0 && urlNum == 0) {
            location = *it;
            return true;
        }
        if (pathNum > urlNum) {
            break ;
        }
        int i = 0;
        while (i < pathNum) {
            if (splitUrl[i] != splitPath[i]) {
                break ;
            }
            if (i + 1 == pathNum) {
                if (i > deepest) {
                    deepest = i;
                    location = *it;
                    ans = true;
                }
            }
            i++;
        }
    }
    std::vector<Location>::const_iterator it = locationList.begin();
    for (; it != locationList.end(); ++it) {
        std::string locPath = it->getPath();
        if (ans == false && urlNum == 1 && ft_split(locPath, '/').size() == 0) {
            location = *it;
            return true;
        }
    }
    return ans;
}

bool Server::findMatchingExtension(std::string & target, Location & myExtension) {
    std::vector<Location> extensionList = config.getExtensionList();
    for (std::vector<Location>::const_iterator it = extensionList.begin(); it != extensionList.end(); ++it) {
        std::string extension = it->getPath();
        if (target.length() >= extension.length()) {
            if (target.compare(target.length() - extension.length(), extension.length(), extension) == 0) {
                myExtension = *it;
                return true;
            };
        }
    }
    return false;
}

void Server::cgiHeaderParsing(std::string & cgiHeader, int & code, std::string &message, std::string & contentType) {
    std::istringstream stream(cgiHeader);
    std::string line;
    if (std::getline(stream, line)) {
        std::istringstream statusLine(line);
        std::string temp;
        statusLine >> temp;
        statusLine >> code;
        std::getline(statusLine, message);
        if (!message.empty() && message[0] == ' ') {
            message = message.substr(1);
        }
    }
    while (std::getline(stream, line)) {
        if (line.find("Content-Type:") == 0) {
            contentType = line.substr(std::string("Content-Type:").length());
            if (!contentType.empty() && contentType[0] == ' ') {
                contentType = contentType.substr(1);
            }
            break;
        }
    }
}

int Server::afterProcessRequest(Buffer *file, struct kevent &change)
{
    std::map<Buffer *, std::pair<Buffer *, HttpRequest*> >::iterator it = requestList.find(file);
    if (it != requestList.end()) {
        if (it->second.second->fileCount == 1)
        {
            Buffer *res = it->second.first;
            std::vector<char> body;
            int code;
            std::string message;
            std::string header;

            if (file->whoAmI() == 3)
            {
                Pipe *pipe = static_cast<Pipe *>(file);
                code = 0;
                if (pipe->getError() == true)
                {
                    file->getReadBuffer().clear();
                    code = 500;
                }
                RequestParser parser;
                std::string target = "\r\n\r\n";
                size_t endHeader = parser.kmp(file->getReadBuffer(), target, 0);
                if (code == 500 || endHeader == RequestParser::npos)
                {
                    code = 500;
                    message = "Internal Server Error";
                    std::string errorPage;
                    if (it->second.second->errorPages.find(code) != it->second.second->errorPages.end()) {
                        errorPage = it->second.second->errorPages[code];       
                    }
                    else {
                        errorPage = makeDefaultErrorPage(code, message).c_str();
                    }
                    body.insert(body.end(), errorPage.begin(), errorPage.end());
                    std::string contentType = "text/html";
                    header = makeHeader(*(it->second.second), code, message, body, contentType);
                    res->getWriteBuffer().reserve(header.size() + body.size());
                    res->getWriteBuffer().insert(res->getWriteBuffer().end(), header.begin(), header.end());
                    res->getWriteBuffer().insert(res->getWriteBuffer().end(), body.begin(), body.end());
                }
                else
                {
                    CgiParser cgiParser(file->getReadBuffer(), endHeader);
                    try
                    {
                        HttpRequest cgiResponse = cgiParser.parse(code, message);
                        for (std::map<std::string, std::string>::iterator tempIt = cgiResponse.headers.begin(); tempIt != cgiResponse.headers.end(); tempIt++)
                        {
                            it->second.second->headers[tempIt->first] = tempIt->second;
                        }
                        if (code / 100 == 4) {
                            std::string errorPage;
                            if (it->second.second->errorPages.find(code) != it->second.second->errorPages.end()) {
                                errorPage = it->second.second->errorPages[code];       
                            }
                            else {
                                errorPage = makeDefaultErrorPage(code, message);
                            }
                            file->getReadBuffer().reserve(file->getReadBuffer().size() + errorPage.size());
                            file->getReadBuffer().insert(file->getReadBuffer().end(), errorPage.begin(), errorPage.end());
                        }
                        std::ostringstream oss;
                        oss << it->second.second->httpVersion << " " << code << " " << message << "\r\n";
                        if (it->second.second->headers.find("Content-Type") != it->second.second->headers.end())
                            oss << "Content-Type: " << it->second.second->headers["Content-Type"] << "\r\n";
                        else
                            oss << "Content-Type: " << it->second.second->contentType << "\r\n";
                        oss << "Content-Length: " << file->getReadBuffer().size() - (endHeader + 4) << "\r\n";
                        oss << "Connection: keep-alive\r\n";
                        if (code / 100 == 3) {
                            oss << "Location: " << it->second.second->url << "\r\n";
                        }
                        oss << "\r\n";
                        header = oss.str();
                        res->getWriteBuffer().reserve(header.size() + file->getReadBuffer().size() - endHeader - 4);
                        res->getWriteBuffer().insert(res->getWriteBuffer().end(), header.begin(), header.end());
                        res->getWriteBuffer().insert(res->getWriteBuffer().end(), file->getReadBuffer().begin() + endHeader + 4, file->getReadBuffer().end());
                    }
                    catch (const std::runtime_error &e)
                    {
                        message = "Internal Server Error";
                        code = 500;
                        std::string errorPage;
                        if (it->second.second->errorPages.find(code) != it->second.second->errorPages.end()) {
                            errorPage = it->second.second->errorPages[code];       
                        }
                        else {
                            errorPage = makeDefaultErrorPage(code, message);
                        }
                        body.insert(body.end(), errorPage.begin(), errorPage.end());
                        std::string contentType = getContentType(it->second.second->url);
                        header = makeHeader(*(it->second.second), code, message, body, contentType);
                        res->getWriteBuffer().reserve(header.size() + body.size());
                        res->getWriteBuffer().insert(res->getWriteBuffer().end(), header.begin(), header.end());
                        res->getWriteBuffer().insert(res->getWriteBuffer().end(), body.begin(), body.end());
                    }
                }
            }
            else // single file || after cgi process temp file
            {
                std::string contentType;
                
                if ((static_cast<File *>(file))->getError() == true || it->second.second->error == true)
                {
                    code = 500;
                    message = "Internal Server Error";
                    file->getReadBuffer().clear();
                }
                else if (it->second.second->method == GET) 
                {
                    code = 200;
                    message = "OK";
                } 
                else if (it->second.second->method == POST) 
                {
                    code = 201;
                    message = "Created";
                }
                contentType = getContentType(it->second.second->url);
                header = makeHeader(*(it->second.second), code, message, file->getReadBuffer(), contentType);
                res->getWriteBuffer().reserve(header.size() + file->getReadBuffer().size());
                res->getWriteBuffer().insert(res->getWriteBuffer().end(), header.begin(), header.end());
                res->getWriteBuffer().insert(res->getWriteBuffer().end(), file->getReadBuffer().begin(), file->getReadBuffer().end());
            }
            Message *client = static_cast<Message*>(it->second.first);
            if (client->getStatus() == WILL_BE_CLOSE)
            {
                client->setStatus(CLOSE);
            }
            client->deleteResource(file);
            EV_SET(&change, res->getFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
            delete it->second.second;
            requestList.erase(file);
            return 1;
        }
        else
        {
            if ((static_cast<File *>(file))->getError() == true)
            {
                it->second.second->error = true;
            }
            it->second.second->fileCount -= 1;
            static_cast<Message*>(it->second.first)->deleteResource(file);
            requestList.erase(file);
        }
    }
    return 0;
}

int Server::checkValid(HttpRequest & request, std::string & target) {
    if (request.method == NONE || request.url == "" || request.httpVersion == "")
    {
        return 400;
    }
    Location myExtension;
    if (findMatchingExtension(request.url, myExtension))
    {
        target = myExtension.getRoot();
        target += '/';
        target += myExtension.getIndex();
        request.url = target;
        return 200;
    }
    Location myLocation;
    bool isLocation = findMatchingLocation(request.url, myLocation);
    if (isLocation == false) {
        if (config.getErrorPages().size())
            request.errorPages = config.getErrorPages();
        if (config.getRedirection().first != 0)
            request.redirection = config.getRedirection();
        if (config.getClientMaxBodySize() < (request.bodyEnd - request.bodyStart))
        {
            return 413;
        }
        if (config.getRoot() != "")
        {
            target += config.getRoot();
        } else
        {
            return 404;
        }
        target += request.url;
        int isDir = isDirectory(target);
        if (isDir == 3)
        {
            if (request.url[request.url.size() - 1] == '/') {
                if (config.getAutoindex() == true) {
                    // request.url = target;
                    return 800;
                }
                else if (config.getIndex() != "") {
                    target += config.getIndex();
                } else {
                    return 404;
                }
            }
        } else if (isDir == 1)
        {
            return 404;
        }
        if (request.method != NONE) {
            std::set<METHOD> allowedMethod = config.getAllowedMethods();
            if (allowedMethod.find(request.method) == allowedMethod.end())
            {
                return 405;
            }
        }
    }
    else
    {
        if (myLocation.getErrorPages().size()) {
            request.errorPages = myLocation.getErrorPages();
        } else if (config.getErrorPages().size()) {
            request.errorPages = config.getErrorPages();
        }
        if (myLocation.getRedirection().first != 0)
            request.redirection = myLocation.getRedirection();
        else if (config.getRedirection().first != 0)
            request.redirection = config.getRedirection();
        if (myLocation.getClientMaxBodySize() != INT_MAX && myLocation.getClientMaxBodySize() < (request.bodyEnd - request.bodyStart)) {
            return 413;
        } else if (config.getClientMaxBodySize() != INT_MAX && config.getClientMaxBodySize() < (request.bodyEnd - request.bodyStart)) {
            return 413;
        }
        if (myLocation.getRoot() != "")
        {
            target += myLocation.getRoot();
        }
        else if (config.getRoot() != "")
        {
            target += config.getRoot();
        }
        else
        {
            return 404;
        }
        target += '/';
        target += request.url.substr(myLocation.getPath().length(), request.url.length());
        int isDir = isDirectory(target);
        if (isDir == 3) {
            if (request.url[request.url.size() - 1] == '/')
            {
                if (myLocation.getAutoindex() || config.getAutoindex()) {
                    // request.url = target;
                    return 800;
                }
                if (myLocation.getIndex() != "") {
                    target += myLocation.getIndex();
                } else if (config.getIndex() != "") {
                    target += config.getIndex();
                } else {
                    return 404;
                }
            } else if (request.method == GET) {
                request.url += '/';
                return 301;
            } else if (request.method == POST) {
                request.url = "/";
                return 201;
            }
        } else if (isDir == 1) {
            return 404;
        }
        if (request.method != NONE) {
            std::set<METHOD> allowedMethod;
            if (myLocation.getAllowedMethods().size()) {
                allowedMethod = myLocation.getAllowedMethods();
            } else if (config.getAllowedMethods().size()) {
                allowedMethod = config.getAllowedMethods();
            }
            if (allowedMethod.find(request.method) == allowedMethod.end()) {
                return 405;
            }
        }
    }
    request.url = target;
    return 200;
}

std::vector<Buffer*> Server::processRequest(Buffer *client, HttpRequest &request, std::vector <struct kevent> &changeList)
{
    std::string target;
    std::vector<Buffer *> resList;
    int code = checkValid(request, target);
    if (code == 200) {
        if (isCGI(target)) {
            if (access(target.c_str(), F_OK | X_OK) == 0) 
            {
                int inputarr[2];
                if (pipe(inputarr) == -1) {
                	throw RuntimeException("pipe");
				}
                int outputarr[2];
                if (pipe(outputarr) == -1) {
                	throw RuntimeException("pipe");
				}
                Pipe *pipe = new Pipe(inputarr[1], outputarr[0]);
                pid_t pid = handleCGI(inputarr, outputarr, target, request);
                pipe->setPid(pid);
                close(inputarr[0]);
                close(outputarr[1]);
                setNonblock(inputarr[1]);
                setNonblock(outputarr[0]);
                struct kevent procEvent;
                EV_SET(&procEvent, pid, EVFILT_PROC, EV_ADD | EV_ENABLE, NOTE_EXIT, 0, (void *)pipe);
                changeList.push_back(procEvent);
                static_cast<Message *>(client)->addResource(pipe);
                pushEvent(inputarr[1], EVFILT_WRITE, EV_ADD | EV_ENABLE, changeList);
                pushEvent(outputarr[0], EVFILT_READ, EV_ADD | EV_ENABLE, changeList);
                pipe->getReadBuffer().reserve(request.bodyEnd - request.bodyStart);
                pipe->getWriteBuffer().insert(pipe->getWriteBuffer().end(), client->getReadBuffer().begin() + request.bodyStart,
                    client->getReadBuffer().begin() + request.bodyEnd);
                HttpRequest *request_share = new HttpRequest;
                *request_share = request;
                request_share->fileCount = 1;
                request_share->target = target;
                requestList[static_cast<Buffer *>(pipe)] = std::make_pair(client, request_share);
                pipe->setServerFd(serverFd);
                resList.push_back(pipe);
            }
            else
            {
                if (access(target.c_str(), F_OK) != 0) {
                    code = 404;
                } else {
                    code = 403;
                }
            }
        }
        else if (request.method == GET && access(target.c_str(), F_OK | R_OK) == 0) {
            int fileFd = open(target.c_str(), O_RDONLY);
            if (fileFd > 0)
            {
                struct stat st;
                stat(target.c_str(), &st);
                size_t size = st.st_size;
                File *res = new File(fileFd);
                res->setServerFd(serverFd);
                static_cast<Message*>(client)->addResource(res);
                HttpRequest *request_share = new HttpRequest;
                *request_share = request;
                request_share->fileCount = 1;
                requestList[static_cast<Buffer *>(res)] = std::make_pair(client, request_share);
                if (size == 0)
                {
                    struct kevent change;
                    if (afterProcessRequest(res, change) == 1)
                        changeList.push_back(change);
                    close(fileFd);
                    delete res;
                    return resList;
                }
                else
                {
                    resList.push_back(static_cast<Buffer *>(res));
                    pushEvent(fileFd, EVFILT_READ, EV_ADD | EV_ENABLE, changeList);
                }
            }
            else
            {
                code = 500;
            }
        }
        else if (request.method == POST)
        {
            if (request.contentType.compare(0, 20, "multipart/form-data;") == 0) {
                std::string boundary = request.contentType.substr(30, std::string::npos);
                if (boundary.size() != 0)
                {
                    MimeParser parser(boundary, client->getReadBuffer(), request.bodyStart, request.bodyEnd);
                    resList = parser.parse(config.getRoot());
                    if (resList.size() > 0)
                    {
                        HttpRequest *request_share = new HttpRequest;
                        *request_share = request;
                        request_share->fileCount = resList.size();
                        for(size_t i= 0 ; i < resList.size(); i++)
                        {
                            static_cast<Message*>(client)->addResource(resList[i]);
                            static_cast<File*>(resList[i])->setServerFd(serverFd);
                            pushEvent(resList[i]->getFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, changeList);
                            requestList[static_cast<Buffer *>(resList[i])] = std::make_pair(client, request_share);
                        }
                    }
                    else
                        code = 400;
                }
                else
                    code = 400;
            }
            else if (request.contentType.compare(0, 33, "application/x-www-form-urlencoded") == 0)
            {
                code = 501;
            }
            else
            {
                int fileFd = open(target.c_str(), O_RDWR, 0644);
                if (fileFd > 0)
                {
                    File *res = new File(fileFd);
                    res->setServerFd(serverFd);
                    static_cast<Message*>(client)->addResource(res);
                    res->getWriteBuffer().reserve(request.bodyEnd - request.bodyStart);
                    res->getWriteBuffer().insert(res->getWriteBuffer().end(), client->getReadBuffer().begin() + request.bodyStart,
                        client->getReadBuffer().begin() + request.bodyEnd);
                    resList.push_back(static_cast<Buffer *>(res));
                    HttpRequest *request_share = new HttpRequest;
                    *request_share = request;
                    request_share->fileCount = 1;
                    request_share->isCgi = false;
                    requestList[static_cast<Buffer *>(res)] = std::make_pair(client, request_share);
                    pushEvent(fileFd, EVFILT_WRITE, EV_ADD | EV_ENABLE, changeList);
                }
                else {
                    code = 500;
                }
            }
        }
        else if (request.method == DELETE && access(target.c_str(), F_OK) == 0)
        {
            if (std::remove(target.c_str()) < 0)
            {
                code = 500;
            }
            else
            {
                code = 200;
                std::vector<char> header = makeResponseWithNoBody(request, code);
                client->getWriteBuffer().reserve(header.size());
                client->getWriteBuffer().insert(client->getWriteBuffer().end(), header.begin(), header.end());
                pushEvent(client->getFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, changeList);
            }
        }
        else 
        {
            if (request.method == GET && access(target.c_str(), F_OK) == 0) {
                code = 403;
            } else {
                code = 404;
            }
        }
    }
    if (code != 200) 
    {
        std::vector<char> response;
        if (code == 800) {
            code = 200;
            DIR *dp;
            struct dirent *entry;
            std::string body;

            dp = opendir(target.c_str());
            if (dp != 0)
            {
                body = "<html>\r\n<head><title>Index of /</title></head>\r\n<body>\r\n<h1>Index of /</h1><hr><pre>";
                while ((entry = readdir(dp)) != 0)
                {
                    struct stat stbuf;
                    std::string fullpath;
                    fullpath += target;
                    fullpath += entry->d_name;
                    if (stat(fullpath.c_str(), &stbuf) == -1)
                    {
                        code = 500;
                        break;
                    }
                    bool is_dir = S_ISDIR(stbuf.st_mode);
                    std::string filename = entry->d_name;
                    if (is_dir == true)
                    {
                        filename += "/";
                    }
                    body += "<a href=\"";
                    body += filename;
                    body += "\">";
                    body += filename;
                    body += "</a>";
                    char time[100];
                    strftime(time, 100, "%Y-%m-%d %H:%M:%S", localtime(&stbuf.st_mtime));
                    body += "                                        ";
                    body += time;
                    body += "                   -\r\n";
                }
                if (code != 500)
                {
                    body += "</pre><hr></body>\r\n</html>";
                    std::string message = "OK";
                    std::vector<char> tempBody;
                    tempBody.insert(tempBody.end(), body.begin(), body.end());
                    std::string contentType = "text/html";
                    std::string header = makeHeader(request, code, message, tempBody, contentType);
                    response.reserve(tempBody.size() + header.size());
                    response.insert(response.end(), header.begin(), header.end());
                    response.insert(response.end(), tempBody.begin(), tempBody.end());
                }
            }
            else
            {
                code = 500;
            }
        } 
        if (code != 200) {
            if (request.errorPages.find(code) != request.errorPages.end()) {
                std::vector<char> body; 
                body.insert(body.end(), request.errorPages[code].begin(), request.errorPages[code].end());
                std::string message = returnErrorMsg(code);
                std::string contentType = getContentType(request.url);
                std::string header = makeHeader(request, code, message, body, contentType);
                response.reserve(header.size() + body.size());
                response.insert(response.end(), header.begin(), header.end());
                response.insert(response.end(), body.begin(), body.end());
            } else {
                response = makeResponseWithNoBody(request, code);
            }
            
        }
        client->getWriteBuffer().reserve(response.size());
        client->getWriteBuffer().insert(client->getWriteBuffer().end(), response.begin(), response.end());
        pushEvent(client->getFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, changeList);
    }
    Message *message = static_cast<Message *>(client);
    if (message->getStatus() == WILL_BE_CLOSE)
    {
        pushEvent(client->getFd(), EVFILT_READ, EV_DELETE, changeList);
        message->setStatus(CLOSE);
    }
    return resList;
}

std::vector<char> Server::makeResponseWithNoBody(HttpRequest &request, int code) {
	std::ostringstream response;
    std::vector<char> body;
    std::string header;
    std::string message;
    std::vector<char> res;

    std::string contentType = getContentType(request.url);
    if (code / 100 == 3) {
        if (code == 301) {
            message = "Moved Permanently";
        } else if (code == 303) {
            message = "See Other";
        }
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
        } else if (code == 500) {
            message = ERR500;
        } else if (code == 501) {
            message = ERR501;
        }
        std::string temp;
        if (request.errorPages.size() && request.errorPages.find(code) != request.errorPages.end()) {
            temp = request.errorPages[code];
        } else {
            temp = makeDefaultErrorPage(code, message);
        }
        body.reserve(body.size() + temp.size());
        body.insert(body.end(), temp.begin(), temp.end());
        contentType = "text/html";
    }
    header = makeHeader(request, code, message, body, contentType);
    res.reserve(header.size() + body.size());
    res.insert(res.end(), header.begin(), header.end());
    res.insert(res.end(), body.begin(), body.end());
    return res;
}

std::string Server::makeDefaultErrorPage(int & code, std::string & message) {
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

std::string Server::makeHeader(HttpRequest & request, int & code, std::string & message, std::vector<char> &body, std::string & contentType) {
    std::ostringstream oss;
    
    if (request.redirection.first != 0) {
        code = request.redirection.first;
        request.url = request.redirection.second;
    }
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

char** Server::makeEnvp(std::string & target, HttpRequest &request) {
    std::list<char *> envpList;
    std::string envp;
    char *tmp;

    envp = "SERVER_PROTOCOL=";
    envp += request.httpVersion;
    tmp = new char[envp.length() + 1];
    std::strcpy(tmp, envp.c_str());
    envpList.push_back(tmp);

    envp = "GATEWAY_INTERFACE=CGI/1.1";
    tmp = new char[envp.length() + 1];
    std::strcpy(tmp, envp.c_str());
    envpList.push_back(tmp);

    envp = "QUERY_STRING=";
    envp += request.query;
    tmp = new char[envp.length() + 1];
    std::strcpy(tmp, envp.c_str());
    envpList.push_back(tmp);

    envp = "REQUEST_METHOD=";
    if (request.method == GET)
        envp += "GET";
    else if (request.method == POST)
        envp += "POST";
    tmp = new char[envp.length() + 1];
    std::strcpy(tmp, envp.c_str());
    envpList.push_back(tmp);

    envp = "SERVER_PORT=";
    envp += ft_toString(config.getListen());
    tmp = new char[envp.length() + 1];
    std::strcpy(tmp, envp.c_str());
    envpList.push_back(tmp);

    envp = "CONTENT_LENGTH=";
    envp += request.contentLength;
    tmp = new char[envp.length() + 1];
    std::strcpy(tmp, envp.c_str());
    envpList.push_back(tmp);

    envp = "CONTENT_TYPE=";
    envp += request.contentType;
    tmp = new char[envp.length() + 1];
    std::strcpy(tmp, envp.c_str());
    envpList.push_back(tmp);

    envp = "SERVER_NAME=";
    envp += request.host;
    tmp = new char[envp.length() + 1];
    std::strcpy(tmp, envp.c_str());
    envpList.push_back(tmp);

    envp = "PATH_INFO=";
    envp += target;
    tmp = new char[envp.length() + 1];
    std::strcpy(tmp, envp.c_str());
    envpList.push_back(tmp);

    for (std::map<std::string, std::string>::iterator it = request.headers.begin(); it != request.headers.end(); ++it) {
        std::string key = (*it).first;
        if (key != "Host" && key != "User-Agent" && key != "Accept" 
        && key != "Content-Length" && key != "Content-Type") {
            for (size_t i=0; i < key.length(); i++) {
                if (key[i] == '-') {
                    key[i] = '_';
                } else if ('a' <= key[i] && key[i] <= 'z') {
                    key[i] -= 32;
                }
            }
            envp = "HTTP_" + key + "=" + (*it).second;
            tmp = new char[envp.length() + 1];
            std::strcpy(tmp, envp.c_str());
            envpList.push_back(tmp);
        }
    }
    char **envpArgs = new char *[envpList.size() + 1];
    int i = 0;
    for (std::list<char *>::iterator it = envpList.begin(); it != envpList.end(); ++it) {
        envpArgs[i++] = *it;
    }
    envpArgs[i] = NULL;
    return envpArgs;
}

pid_t Server::handleCGI(int inputFileFd[2], int outputFileFd[2], std::string & target, HttpRequest &request) {

    clock_t start;
    pid_t pid = fork();
	start = clock();
	childTime[pid] = start;
    if (pid == -1) {
        throw RuntimeException("fork");
    }
    if (pid == 0) {
        if (dup2(inputFileFd[0], STDIN_FILENO) == -1) {
            throw RuntimeException("dup2");
        }
        if (dup2(outputFileFd[1], STDOUT_FILENO) == -1) {
            throw RuntimeException("dup2");
        }
        close(inputFileFd[1]);
        close(inputFileFd[0]);
        close(outputFileFd[1]);
        close(outputFileFd[0]);
        char *execArgs[2];
        execArgs[0] = const_cast<char *>(target.c_str());
        execArgs[1] = NULL;
        char** envp = makeEnvp(target, request);
        execve(target.c_str(), execArgs, envp);
        arrFree(envp);
        throw RuntimeException("execve");
    } else {
        return (pid);
    }
}

void Server::deleteRequestByFile(Buffer *buf)
{
    std::map<Buffer*, std::pair<Buffer*, HttpRequest*> >::iterator it;

    if (requestList.size() == 0)
        return;
    it = requestList.find(buf);
    if (it != requestList.end())
    {
        if (it->second.second->fileCount == 1)
            delete it->second.second;
        else
            it->second.second->fileCount -= 1;
        requestList.erase(it);
    }
}
