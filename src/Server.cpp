/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 14:42:02 by haejeong          #+#    #+#             */
/*   Updated: 2024/07/12 12:42:40 by haejeong         ###   ########.fr       */
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

bool Server::findMatchingLocation(std::string & requestURL, Location & location) {
    std::vector<Location> locationList = config.getLocationList();
    std::vector<std::string> splitUrl = ft_split(requestURL, '/'); // 0
    int j = splitUrl.size();
    std::vector<std::string> splitPath;
    for (std::vector<Location>::const_iterator it = locationList.begin(); it != locationList.end(); ++it) {
        std::string locPath = it->getPath();
        splitPath = ft_split(locPath, '/');
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
                    location = *it;
                    return true;
                }
            }
            i++;
        }
    }
    std::vector<Location>::const_iterator it = locationList.begin();
    for (; it != locationList.end(); ++it) {
        std::string locPath = it->getPath();
        if (j == 1 && ft_split(locPath, '/').size() == 0) {
            location = *it;
            return true;
        }
    }
    return false;
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
        if (isCGI(target)) {
            if (access(target.c_str(), F_OK | R_OK | X_OK) == 0) {
                int tmpFileFd = open("tmp", O_RDWR | O_CREAT, 0644);
                if (tmpFileFd > 0)
                {
                    handleCGI(tmpFileFd, target, request);
                    close(tmpFileFd);
                    tmpFileFd = open("tmp", O_RDONLY);
                    File *res = new File(tmpFileFd);
                    resList.push_back(static_cast<Buffer *>(res));
                    HttpRequest *request_share = new HttpRequest;
                    *request_share = request;
                    request_share->fileCount = 1;
                    requestList[static_cast<Buffer *>(res)] = std::make_pair(client, request_share);
                    pushEvent(tmpFileFd, EVFILT_READ, EV_ADD | EV_ENABLE, changeList);
                    return resList;
                }
            }
        }
        else if (request.method == GET && access(target.c_str(), F_OK | R_OK) == 0) {
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

void Server::handleCGI(int tmpFileFd, std::string & target, HttpRequest &request) {
    char *const envp[] = {
        (char *)"AUTH_TYPE=",                   // 서블릿을 보호하는 데 사용되는 인증 스키마의 이름입니다. 예를 들면 BASIC, SSL 또는 서블릿이 보호되지 않는 경우 null입니다.
        (char *)"CONTENT_LENGTH=0",             // 입력 스트림에서 사용 가능한 요청 본문의 길이(바이트) 또는 길이를 알 수 없는 경우 -1입니다. HTTP 서블릿의 경우 리턴되는 값은 CGI 변수 CONTENT_LENGTH의 값과 동일합니다.
        (char *)"CONTENT_TYPE=text/html",       // 요청 본문의 MIME 유형 또는 유형을 모르는 경우 null입니다. HTTP 서블릿의 경우 리턴되는 값은 CGI 변수 CONTENT_TYPE의 값과 동일합니다
        (char *)"GATEWAY_INTERFACE=CGI/1.1",    // 서버가 스크립트와 통신하기 위해 사용하는 CGI 스펙의 버전입니다.
        (char *)"HTTP_ACCEPT=",                 // "HTTP_"로 시작하는 이름을 가진 변수는 사용되는 스키마가 HTTP인 경우 요청 헤더의 값을 포함합니다. HTTP_ACCEPT는 브라우저가 지원하는 내용 유형을 지정합니다. 예를 들어, text/xml입니다.
        (char *)"HTTP_ACCEPT_CHARSET=",         // 문자 환경 설정 정보. 정보가 있는 경우 클라이언트의 선호하는 문자 세트를 표시하는 데 사용됩니다. 예를 들어, utf-8;q=0.5입니다.
        (char *)"HTTP_ACCEPT_ENCODINGE=",       // 클라이언트로 리턴되는 내용에 대해 수행될 수 있는 인코딩의 유형을 정의합니다. 예를 들어, compress;q=0.5입니다.
        (char *)"HTTP_ACCEPT_LANGUAGE=",        // 수신할 내용에 적합한 언어를 정의하는 데 사용됩니다. 예를 들어, en;q=0.5입니다. 리턴되는 결과가 없으면 언어 환경 설정이 표시되지 않습니다.
        (char *)"HTTP_FORWARDED=",              // 요청이 전달되는 경우 프록시 서버의 주소 및 포트를 표시합니다.
        (char *)"HTTP_HOST=",                   // 요청되는 자원의 인터넷 호스트 및 포트 번호를 지정합니다. 모든 HTTP/1.1 요청의 경우 필수입니다.
        (char *)"HTTP_PROXY_AUTHORIZATION=",    // 인증이 필요한 프록시에 클라이언트 자체 또는 클라이언트의 사용자를 식별하기 위해 클라이언트가 사용합니다.
        (char *)"HTTP_USER_AGENT=",             // 클라이언트가 요청을 보내기 위해 사용 중인 브라우저의 유형 및 버전입니다. 예를 들어, Mozilla/1.5입니다.
        (char *)"PATH_INFO=/hello.html",        // 선택적으로 스크립트를 호출한 HTTP 요청의 추가 경로 정보를 포함하고 있으며 CGI 스크립트로 해석할 경로를 지정합니다. PATH_INFO는 CGI 스크립트가 리턴할 자원 또는 하위 자원을 식별하며 스크립트 이름 뒤에 오지만 모든 조회 데이터 앞에 오는 URI 경로 부분에서 파생됩니다.
        (char *)"PATH_TRANSLATED=",             // 스크립트의 가상 경로를 스크립트를 호출하는 데 사용되는 실제 경로에 맵핑합니다. 맵핑은 요청 URI의 PATH_INFO 구성요소를 가져와 적합한 가상 대 실제 변환을 수행하여 실행됩니다.
        (char *)"QUERY_STRING=",                // 경로 뒤의 요청 URL에 포함된 조회 문자열입니다.
        (char *)"REMOTE_ADDR=",                 // 요청을 보낸 클라이언트의 IP 주소를 리턴합니다. HTTP 서블릿의 경우 리턴되는 값은 CGI 변수 REMOTE_ADDR의 값과 동일합니다.
        (char *)"REMOTE_HOST=",                 // 요청을 보낸 클라이언트의 완전한 이름이거나 이름을 판별할 수 없는 경우 클라이언트의 IP 주소입니다. HTTP 서블릿의 경우 리턴되는 값은 CGI 변수 REMOTE_HOST의 값과 동일합니다.
        (char *)"REMOTE_USER=",                 // 사용자가 인증된 경우 이 요청을 작성한 사용자의 로그인을 리턴하고 사용자가 인증되지 않은 경우 널(null)을 리턴합니다.
        (char *)"REQUEST_METHOD=GET",           // 이 요청을 작성할 때 사용된 HTTP 메소드의 이름을 리턴합니다. 예를 들어, GET, POST 또는 PUT입니다.
        (char *)"SCRIPT_NAME=",                 // 프로토콜 이름에서 HTTP 요청의 첫 번째 라인에 있는 조회 문자열까지, URL의 부분을 리턴합니다.
        (char *)"SERVER_NAME=localhost",        // 요청을 수신한 서버의 호스트 이름을 리턴합니다. HTTP 서블릿의 경우 CGI 변수 SERVER_NAME의 값과 동일합니다.
        (char *)"SERVER_PORT=80",               // 이 요청이 수신된 포트 번호를 리턴합니다. HTTP 서블릿의 경우 리턴되는 값은 CGI 변수 SERVER_PORT의 값과 동일합니다.
        (char *)"SERVER_PROTOCOL=HTTP/1.1",     // 요청이 사용하는 프로토콜의 이름과 버전을 protocol/majorVersion.minorVersion 양식으로 리턴합니다. 예를 들어, HTTP/1.1입니다. HTTP 서블릿의 경우 리턴되는 값은 CGI 변수 SERVER_PROTOCOL의 값과 동일합니다.
        (char *)"SERVER_SOFTWARE=versbew",      // 서블릿이 실행 중인 서블릿 컨테이너의 이름과 버전을 리턴합니다.
        (char *)"HTTP_COOKIE=",                 // HTTP 쿠키 문자열
        (char *)"WEBTOP_USER=",                 // 로그인한 사용자의 사용자 이름
        (char *)"NCHOME=",                      // NCHOME 환경 변수
        NULL
    };

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        throw RuntimeException("pipe");
    }

    pid_t pid = fork();
    if (pid == -1) {
        throw RuntimeException("fork");
    }

    if (pid == 0) {

        if (dup2(pipefd[0], STDIN_FILENO) == -1) {
            throw RuntimeException("dup2");
        }
        if (dup2(tmpFileFd, STDOUT_FILENO) == -1) {
            throw RuntimeException("dup2");
        }
        close(pipefd[0]);
        close(pipefd[1]);

        std::vector<char *> exec_args;
        exec_args.push_back(const_cast<char *>(target.c_str()));
        exec_args.push_back(NULL);
        execve(target.c_str(), exec_args.data(), envp);
        throw RuntimeException("execve");

    } else { // 부모 프로세스
        write(pipefd[1], request.query.c_str(), request.query.length());
        close(pipefd[0]);
        close(pipefd[1]);

        int status;
        waitpid(pid, &status, 0);
        if (!WIFEXITED(status)) {
            throw RuntimeException("chid process err");
        }
    }
}
