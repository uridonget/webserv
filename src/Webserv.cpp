/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 13:01:20 by haejeong          #+#    #+#             */
/*   Updated: 2024/07/12 12:42:56 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Webserv.hpp"
#include "../include/Server.hpp"

Webserv::Webserv() : kq(0) {}

Webserv::~Webserv() {
	for (int i = 0; i < bufferList.size(); i++)
	{
		close(bufferList[i]->getFd());
		delete bufferList[i];
	}
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
	}
	return eventIdent;
}

bool Webserv::checkSocketError(int idx) {
	if (eventList[idx].flags & EV_ERROR) {
		if (checkNewClient(eventList[idx].ident) != -1) {
			throw RuntimeException("server socket");
		} else {
			return true ;
		}
	}
	return false ;
}

void Webserv::newClient(int serverFd) {
	int clientFd = accept(serverFd, NULL, NULL);
	if (clientFd < 0) {
		std::cout << "serecrFD : " << serverFd<< std::endl;
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
	if (!request.query.empty()) {
        std::cout << "Query: " << request.query << std::endl;
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
        std::cout << "Body:\n";
        for (std::vector<char>::const_iterator it = request.body.begin(); it != request.body.end(); ++it) {
            std::cout << *it;
        }
    }
}

void Webserv::readEvent(int idx, int bufferIdx, int serverFd) {
	if (eventList[idx].flags & EV_EOF) { // EOF
		if (isMessage(bufferIdx) == 1) { // message EOF : something wrong
			closeSocket(bufferIdx);
			return ;
		}
		// File == 2
		else if (isMessage(bufferIdx) == 2) {
			// undefine in kqueue manual
			// this case cannot be happend
			return ;
		}
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
	if (isMessage(bufferIdx) == 2) { // File
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
		return ;
	}
	// Message == 1
	RequestParser parser;
	size_t endHeader;
	Message *message = dynamic_cast<Message *>(bufferList[bufferIdx]);
	size_t endIndex = parser.checkEnd(message, buf, n, endHeader);
    if (endIndex != RequestParser::npos) {
		Buffer *buffer = bufferList[bufferIdx];
		llParser llparser(bufferList[bufferIdx]->getReadBuffer(), endHeader);
		try{
			HttpRequest request = llparser.parse(); // 파싱 완료
			// 여기에서 터짐
			request.body.insert(request.body.end(), buffer->getReadBuffer().begin() + endHeader + 4, buffer->getReadBuffer().begin() + endIndex + 4);
			// 여기에서 터짐
			printHttpRequest(request);
			std::map<int, Server>::iterator server = serverList.find(serverFd);
			if (server != serverList.end())
			{
				std::vector<struct kevent> clientEvent;
				std::vector<Buffer *> file;
				file = server->second.processRequest(bufferList[bufferIdx], request, clientEvent);
				for (int i = 0; i < file.size(); i++)
				{
					bufferList.push_back(file[i]);
					serverFdMap[file[i]->getFd()] = serverFd;
				}
				changeList.insert(changeList.end(), clientEvent.begin(), clientEvent.end());
			}
		}
		catch (const std::runtime_error &e)
		{
			std::cerr << "Parsing error: " << e.what() << std::endl;
		}
		buffer->getReadBuffer().erase(buffer->getReadBuffer().begin(), buffer->getReadBuffer().begin() + endIndex + 4);
	}
}

void Webserv::writeEvent(int idx, int bufferIdx, int serverFd) {
	// std::cout << "****** write event ******" << std::endl;

	std::vector<char>& writeBuffer = bufferList[bufferIdx]->getWriteBuffer();
	int writeSize = BUFFER_SIZE;
	if (writeBuffer.size() < BUFFER_SIZE) {
		writeSize = writeBuffer.size();
	}
	int writtenSize = write(bufferList[bufferIdx]->getFd(), writeBuffer.data(), writeSize);
	if (writtenSize == -1)
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
	if (writtenSize > 0)
		writeBuffer.erase(writeBuffer.begin(), writeBuffer.begin() + writtenSize);
	if (!writeBuffer.empty()){
		return ;
	}

	// Message == 1
	// 응답 보냈다는 뜻
	if (isMessage(bufferIdx) == 1) {
		// 메시지 전송 성공
		bufferList[bufferIdx]->getWriteBuffer().clear();
		successResponse(bufferIdx);
		return ;
	}
	if (isMessage(bufferIdx) == 2) {
		// 파일 전송 성공
		bufferList[bufferIdx]->getWriteBuffer().clear();
		std::map<int, Server>::iterator server = serverList.find(serverFd);  
		struct kevent clientEvent;
		if (server->second.afterProcessRequest(bufferList[bufferIdx], clientEvent) == 1)
			changeList.push_back(clientEvent);
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
			// if (checkSocketError(i)) {
			// 	continue;
			// }
			if (eventList[i].flags & EV_DELETE) {
				continue;
			}
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
			if (eventList[i].filter == EVFILT_READ) {
				int serverFd = serverFdMap.find(eventList[i].ident)->second;
				readEvent(i, j, serverFd);
			} else if (eventList[i].filter == EVFILT_WRITE) {
				int serverFd = serverFdMap.find(eventList[i].ident)->second;
				writeEvent(i, j, serverFd);
			}
		}
	}
}
