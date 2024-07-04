/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 13:01:20 by haejeong          #+#    #+#             */
/*   Updated: 2024/07/04 18:43:18 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Webserv.hpp"
#include "../include/Server.hpp"

Webserv::Webserv() : kq(0) {}

Webserv::~Webserv() {}

void Webserv::closeSocket(int bufferIdx) {
	struct kevent clientEvent1;
	EV_SET(&clientEvent1, bufferList[bufferIdx]->getFd(), EVFILT_READ, EV_DELETE, 0, 0, NULL);
	changeList.push_back(clientEvent1);
	struct kevent clientEvent2;
	EV_SET(&clientEvent2, bufferList[bufferIdx]->getFd(), EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
	changeList.push_back(clientEvent2);
	close(bufferList[bufferIdx]->getFd());
	delete bufferList[bufferIdx];
	bufferList.erase(bufferList.begin() + bufferIdx);
}

void Webserv::closeFile(int bufferIdx) {
	struct kevent fileEvent;
	EV_SET(&fileEvent, bufferList[bufferIdx]->getFd(), EVFILT_READ, EV_DELETE, 0, 0, NULL);
	changeList.push_back(fileEvent);
	close(bufferList[bufferIdx]->getFd());
}

void Webserv::successResponse(int bufferIdx) {
	struct kevent clientEvent;
	EV_SET(&clientEvent, bufferList[bufferIdx]->getFd(), EVFILT_WRITE, EV_DELETE , 0, 0, NULL);
	changeList.push_back(clientEvent);
}

void Webserv::successFileWrite(int bufferIdx) {
	struct kevent clientEvent;
	EV_SET(&clientEvent, bufferList[bufferIdx]->getFd(), EVFILT_WRITE, EV_DELETE , 0, 0, NULL);
	changeList.push_back(clientEvent);
	delete bufferList[bufferIdx];
	bufferList.erase(bufferList.begin() + bufferIdx);
	close(bufferList[bufferIdx]->getFd());
}

void Webserv::configurationParsing(const std::string & configPath) {
	std::ifstream configFile(configPath.c_str());
	if (!configFile.is_open()) {
		throw RuntimeException("Invalid configuration file path");
	}
	std::stringstream fileBuffer;
	fileBuffer << configFile.rdbuf();
	configFile.close();
	std::string fileInput = fileBuffer.str();
	configParsing.removeComment(fileInput);
	configParsing.removeEmptyLine(fileInput);
	configParsing.splitServer(fileInput);
	configParsing.setServerConfig();
}

void Webserv::initKqueue() {
	kq = kqueue();
	if (kq < 0) {
		throw RuntimeException("kqueue");
	}
}

void Webserv::makeServerList() {
	std::vector<ServerConfig> configs = configParsing.getServerConfigs();
	for (std::vector<ServerConfig>::iterator it = configs.begin(); it != configs.end(); it++) {
		Server server;
		server.initServer(*it);
		serverList.insert(std::make_pair(server.getServerFd(), server));
	}
}

void Webserv::registerServerWithChangeList() {
	for (std::map<int, Server>::iterator it = serverList.begin(); it != serverList.end(); ++it) {
		struct kevent serverEvent;
		EV_SET(&serverEvent, (*it).second.getServerFd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		changeList.push_back(serverEvent);
	}
}

int Webserv::checkNewClient(uintptr_t eventIdent) {
	std::map<int, int>::iterator it = serverFdMap.find(eventIdent);
	if (it != serverFdMap.end()) {
		return -1;
	} else {
		for (std::vector<Buffer *>::iterator it = bufferList.begin(); it != bufferList.end(); ++it) {
			if ((*it)->getFd() == eventIdent) {
				return -1;
			}
		}
	}
	return static_cast<int>(eventIdent);
}

bool Webserv::checkSocketError(int idx) {
	if (eventList[idx].flags & EV_ERROR) {
		std::cout << "EV_ERROR" << std::endl;
		if (checkNewClient(eventList[idx].ident)) {
			throw RuntimeException("server socket");
		} else {
			return true ;
		}
	}
	return false ;
}

void Webserv::newClient(int serverFd) {
	std::cout << "WHAT IS THE SERVER FD : " << serverFd << std::endl;
	int clientFd = accept(serverFd, NULL, NULL);
	if (clientFd < 0) {
		std::cout << "serecrFD : " << serverFd<< std::endl;
		system("lsof -c webserv");
		perror("accept");
		throw RuntimeException("accept");
	} else {
		serverFdMap.insert(std::make_pair(clientFd, serverFd));
		setNonblock(clientFd);
		struct kevent clientEvent;
		EV_SET(&clientEvent, clientFd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		changeList.push_back(clientEvent);
		Buffer* newClient = new Message(clientFd);
		bufferList.push_back(newClient);
	}
}

int Webserv::isMessage(int bufferIdx) {
	// 1 : message
	// 2 : file
	return bufferList[bufferIdx]->whoAmI();
}

void printHttpRequest(const HttpRequest& request) {
    std::cout << "\n<REQUEST>" << std::endl;
    if (request.method != NONE) {
        std::cout << "Method: ";
		if (request.method == GET)
			std::cout << "GET" << std::endl;
		else if (request.method == POST)
			std::cout << "POST" << std::endl;
		else if (request.method == DELETE)
			std::cout << "DELETE" << std::endl;
    }
    if (!request.url.empty()) {
        std::cout << "URL: " << request.url << std::endl;
    }
    if (!request.httpVersion.empty()) {
        std::cout << "HTTP Version: " << request.httpVersion << std::endl;
    }
    if (!request.host.empty()) {
        std::cout << "Host: " << request.host << std::endl;
    }
    if (!request.userAgent.empty()) {
        std::cout << "User-Agent: " << request.userAgent << std::endl;
    }
    if (!request.accept.empty()) {
        std::cout << "Accept: " << request.accept << std::endl;
    }
    if (!request.contentLength.empty()) {
        std::cout << "Content-Length: " << request.contentLength << std::endl;
    }
    if (!request.body.empty()) {
        std::cout << "Body: ";
        for (std::vector<char>::const_iterator it = request.body.begin(); it != request.body.end(); ++it) {
            std::cout << *it;
        }
        std::cout << std::endl;
    }
}

void Webserv::readEvent(int idx, int bufferIdx, int serverFd) {
	std::cout << "****** read event ******" << std::endl;
	if (eventList[idx].flags & EV_EOF) {
		if (isMessage(bufferIdx) == 1) { // message EOF
			closeSocket(bufferIdx);
		}
		// File == 2
		else if (isMessage(bufferIdx) == 2) {
			// 여기는 파일 다 읽어서 클라이언트로 보낸다는거임
			std::cout << "file read end!" << std::endl;
			closeFile(bufferIdx);
			std::map<int, Server>::iterator server = serverList.find(serverFd);
			if (server != serverList.end())
			{
       			struct kevent client_event;
				server->second.afterProcessRequest(bufferList[bufferIdx], client_event);
				serverFdMap.erase(bufferList[bufferIdx]->getFd());
				delete dynamic_cast<File *>(bufferList[bufferIdx]);
				bufferList.erase(bufferList.begin() + bufferIdx);
				changeList.push_back(client_event);
			}
			return ;
		}
		return ;
	}

	// EOF NO
	int clientFd = eventList[idx].ident; 
	char buf[BUFFER_SIZE] = {0};
	int n = read(clientFd, buf, BUFFER_SIZE);
	if (n < 0) {
		delete bufferList[bufferIdx];
		bufferList.erase(bufferList.begin() + bufferIdx);
		close(clientFd);
		struct kevent clientEvent;
		EV_SET(&clientEvent, clientFd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
		changeList.push_back(clientEvent);
		return ;
	}

	// File == 2
	if (isMessage(bufferIdx) == 2) {
		bufferList[bufferIdx]->getReadBuffer().insert(bufferList[bufferIdx]->getReadBuffer().end(), buf, buf + n);
		if (n != BUFFER_SIZE || n == 0)
		{
			closeFile(bufferIdx);
			std::map<int, Server>::iterator server = serverList.find(serverFd);
			if (server != serverList.end())
			{
       			struct kevent client_event;
				server->second.afterProcessRequest(bufferList[bufferIdx], client_event);
				serverFdMap.erase(bufferList[bufferIdx]->getFd());
				delete dynamic_cast<File *>(bufferList[bufferIdx]);
				bufferList.erase(bufferList.begin() + bufferIdx);
				changeList.push_back(client_event);
			}
		}
		// 파일을 덜 읽었다는 뜻임
		return ;
	}

	// Message == 1
	// 여기서 쭉쭉 내려가면 됨
	RequestParser parser;
	size_t endHeader;
	size_t endIndex = parser.checkEnd(bufferList[bufferIdx]->getReadBuffer(), buf, n, endHeader);
    if (endIndex != RequestParser::npos)
    {
		Buffer *buffer = bufferList[bufferIdx];
		llParser parser(bufferList[bufferIdx]->getReadBuffer(), endHeader);
		try{
			HttpRequest request = parser.parse();
			std::map<int, Server>::iterator server = serverList.find(serverFd);
			if (server != serverList.end())
			{
       			struct kevent client_event;
				Buffer *file = server->second.processRequest(bufferList[bufferIdx], request, client_event);
				if (file)
				{
					bufferList.push_back(file);
					serverFdMap[file->getFd()] = serverFd;
				}
				changeList.push_back(client_event);
			}
		}
		catch (const std::runtime_error &e)
		{
			std::cerr << "Parsing error: " << e.what() << std::endl;
		}
		std::vector<char> temp =  std::vector<char>(buffer->getReadBuffer().begin() + endIndex + 4, buffer->getReadBuffer().end());
		buffer->getReadBuffer().clear();
		buffer->getReadBuffer() = temp;
	}
}

void Webserv::writeEvent(int idx, int bufferIdx, int serverFd) 
{
	// 일단 write하셈 ㅋㅋ
	int writeSize = write(bufferList[bufferIdx]->getFd(), bufferList[bufferIdx]->getWriteBuffer().data(), bufferList[bufferIdx]->getWriteBuffer().size());
	if (writeSize == -1)
	{
		if (close(bufferList[bufferIdx]->getFd()) == -1)
			std::cout << "already closed fd!!!" << std::endl;
		delete bufferList[bufferIdx];
		bufferList.erase(bufferList.begin() + bufferIdx);
		struct kevent clientEvent1;
		struct kevent clientEvent2;
		EV_SET(&clientEvent1, bufferList[bufferIdx]->getFd(), EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
		EV_SET(&clientEvent2, bufferList[bufferIdx]->getFd(), EVFILT_READ, EV_DELETE, 0, 0, NULL);
		changeList.push_back(clientEvent1);
		changeList.push_back(clientEvent2);
		return ;
	}
	if (writeSize != bufferList[bufferIdx]->getWriteBuffer().size()){
		// 여기서 사이즈가 서로 다르다는건 버퍼 사이즈 한계로 다 못 썼다는 것
		// 이터레이터로 어디까지 썼는지 알고 있어야 함
		return ;
	}
	if (isMessage(bufferIdx) == 1) {
		// 메시지 전송 성공
		successResponse(bufferIdx);
		return ;
	}
	if (isMessage(bufferIdx) == 2) {
		// 파일 전송 성공
		successFileWrite(bufferIdx);
		return ;
	}
}

void Webserv::runServers() {
	struct timespec timeout;
	timeout.tv_sec = 10;
	timeout.tv_nsec = 0;

	signal(SIGPIPE, SIG_IGN);
	while (true) {
		int nev = kevent(kq, &changeList[0], changeList.size(), eventList, 10, &timeout);
		if (nev < 0) {
			throw RuntimeException("kevent");
		}
		changeList.clear();
		if (nev == 0) {
			continue;
		}
		for (int i=0; i < nev; i++) {
			int isNew = checkNewClient(eventList[i].ident);
			if (isNew > 0) {
				newClient(eventList[i].ident);
				continue ;
			}
			int j = 0;
			for (; j < bufferList.size(); j++) {
				if (bufferList[j]->getFd() == eventList[i].ident) {
					break;
				}
			}
			if (checkSocketError(i)) {
				continue;
			}
			if (eventList[i].flags & EV_DELETE) {
				continue;
			} else if (eventList[i].flags & EV_EOF) {
				delete bufferList[j];
				bufferList.erase(bufferList.begin() + j);
				close(eventList[i].ident);
				// struct kevent clientEvent;
				// EV_SET(&clientEvent, eventList[i].ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
				// changeList.push_back(clientEvent);
				continue;
			} else if (eventList[i].filter == EVFILT_READ) {
				int serverFd = serverFdMap.find(eventList[i].ident)->second;
				// std::cout << "----- server port : " << serverFd << " -----" << std::endl;
				readEvent(i, j, serverFd);
			} else if (eventList[i].filter == EVFILT_WRITE) {
				int serverFd = serverFdMap.find(eventList[i].ident)->second;
				// std::cout << "----- server port : " << serverFd << " -----" << std::endl;
				writeEvent(i, j, serverFd);
			}
		}
	}
}



