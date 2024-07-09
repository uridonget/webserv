/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 13:01:20 by haejeong          #+#    #+#             */
/*   Updated: 2024/07/04 16:58:12 by sangyhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Webserv.hpp"
#include "../include/Server.hpp"

Webserv::Webserv() : kq(0)
{
	// std::cout << "Default Webserv constructor called" << std::endl;
}

Webserv::~Webserv()
{
	// std::cout << "Webserv destructor called" << std::endl;
}

void Webserv::makeServerConfigStringList(const std::string & configPath) {
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

std::vector<ServerConfig> Webserv::getServerConfigs() {
	return (this->configParsing.getServerConfigs());
}

void Webserv::makeServerList() {
	std::vector<ServerConfig> configs;

	configs = this->configParsing.getServerConfigs();
	for (std::vector<ServerConfig>::iterator it = configs.begin(); it != configs.end(); it++) {
		Server serv;
		serv.initServer(*it);
		// serverFdSet.insert(serv.getServerFd());
		serverList.insert(std::make_pair(serv.getServerFd(), serv));
	}
}

int Webserv::checkNewClient(uintptr_t eventIdent) {
	std::map<int, int>::iterator it = serverFdSet.find(eventIdent);
	if (it != serverFdSet.end()) { // server fd가 아니다! new client 아님!
		// std::cout << "this is old client!" << std::endl;
		return -1;
	}
	else {
		for (std::vector<Buffer *>::iterator it = bufferList.begin(); it != bufferList.end(); it++)
		{
			if ((*it)->getFd() == eventIdent)
			{
				return -1;
			}
		}
		// std::cout << "this is new client!" << std::endl;
		return eventIdent; // new client
	}
}

bool Webserv::checkSocketError(int idx, int bufferIdx) {
	if (eventList[idx].flags == EV_ERROR) {
		std::cerr << "***************** EV_ERROR ENTERED *****************" << std::endl; 
		if (checkNewClient(eventList[idx].ident)) {
			throw RuntimeException("Server socket");
		} else {
			// close(eventList[idx].ident);
			// delete bufferList[bufferIdx];
			// bufferList.erase(bufferList.begin() + bufferIdx);
			// clients.erase(eventList[idx].ident);
			return true;
		}
	}
	return false;
}


void Webserv::runServers()
{
	struct timespec timeout;
	timeout.tv_sec = 5;
	timeout.tv_nsec = 0;

	signal(SIGPIPE, SIG_IGN);
	int cnt = 0;
	while (true)
	{
		int nev = kevent(kq, &changeList[0], changeList.size(), eventList, 10, &timeout);
		if (nev < 0) {
			throw RuntimeException("kevent");
		}
		changeList.clear();
		if (nev == 0) {
			std::cout << "NO EVENT" << std::endl;
			continue;
		}
		for (int i = 0; i < nev; i++)
		{
			int j = 0;
			for (; j < bufferList.size(); j++) {
				if (bufferList[j]->getFd() == eventList[i].ident) {
					break;
				}
			}
			if (checkSocketError(i, j)) {
				// close(eventList[i].ident);
				continue; // error check
			}
			if (eventList[i].flags & EV_DELETE)
			{
				continue;
			}
			int temp = checkNewClient(eventList[i].ident);
			if (temp > 0)
			{
				newClient(eventList[i].ident); // new client
			}
			else if (eventList[i].filter == EVFILT_READ)
			{
				int serverFd = serverFdSet.find(eventList[i].ident)->second;
				readEvent(i, j, serverFd);
			}
			else if (eventList[i].filter == EVFILT_WRITE)
			{
				int serverFd = serverFdSet.find(eventList[i].ident)->second;
				writeEvent(i, j, serverFd);
			}
		}
	}
}

void Webserv::initKqueue() {
	kq = kqueue();
	if (kq < 0) {
		throw RuntimeException("kqueue");
	}
}

void Webserv::newClient(int serverFd) {
	int clientFd = accept(serverFd, NULL, NULL);
	if (clientFd < 0) {
		std::cout << "serecrFD : " << serverFd<< std::endl;
		throw RuntimeException("accept");
	} else {
		serverFdSet.insert(std::make_pair(clientFd, serverFd));
		setNonblock(clientFd);
		struct kevent client_event;
		EV_SET(&client_event, clientFd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		changeList.push_back(client_event);
		Buffer* newClient = new Message(clientFd);
		bufferList.push_back(newClient);
	}
}

void Webserv::readEvent(int idx, int bufferIdx, int serverFd) 
{
	// EOF YES
	if (eventList[idx].flags & EV_EOF) 
	{
		// Message == 1
		if (isMessage(bufferIdx) == 1) {

			// 소켓이 닫혔다는 거임 그거 버려야함
			closeSocket(bufferIdx);
			return ;
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
				serverFdSet.erase(bufferList[bufferIdx]->getFd());
				delete dynamic_cast<File *>(bufferList[bufferIdx]);
				bufferList.erase(bufferList.begin() + bufferIdx);
				changeList.push_back(client_event);
			}
			return ;
		}
	}

	// EOF NO
	int clientFd = eventList[idx].ident; 
	char buf[BUFFER_SIZE] = {0};
	int n = read(clientFd, buf, BUFFER_SIZE);
	// 예외처리
	if (n == -1)
	{
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
				serverFdSet.erase(bufferList[bufferIdx]->getFd());
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
				if (file != 0)
				{
					bufferList.push_back(static_cast<Buffer *>(file));
					serverFdSet[file->getFd()] = serverFd;
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


void Webserv::writeEvent(int idx, int bufferIdx, int serverFd) {
	// std::cout << "****** write event ******" << std::endl;

	std::vector<char>& writeBuffer = bufferList[bufferIdx]->getWriteBuffer();
	std::string response(writeBuffer.begin(), writeBuffer.end());

	int writeSize = BUFFER_SIZE;
	if (writeBuffer.size() < BUFFER_SIZE) {
		writeSize = writeBuffer.size();
	}

	int writtenSize = write(bufferList[bufferIdx]->getFd(), response.c_str(), writeSize);

	for (int i = 0; i < writtenSize; i++) {
		writeBuffer.erase(writeBuffer.begin());
	}

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

	// 여기서 사이즈가 서로 다르다는건 버퍼 사이즈 한계로 다 못썼다는 뜻 다시ㄱㄱ
	// 이터레이터로 어디까지 썼는지 알고 있어야 함
	if (!writeBuffer.empty()){
		return ;
	}


	// Message == 1
	// 응답 보냈다는 뜻
	if (isMessage(bufferIdx) == 1) {

		successResponse(bufferIdx);
		return ;
	}

	// File == 2
	// 파일에 쓴다 POST 요청 성공 이라는 뜻
	// successFileWrite(int bufferIdx);

	// 서버 구현 필요!
	// 여기서 서버가 응답 메세지 생성 해줘야함

	// return;

}

void Webserv::connectKqueueToServer() {
	for (size_t i = 0; i < serverList.size(); i++) {
		struct kevent ke;
		EV_SET(&ke, serverList[i].getServerFd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		changeList.push_back(ke);
	}
}
