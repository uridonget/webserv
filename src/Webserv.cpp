/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 13:01:20 by haejeong          #+#    #+#             */
/*   Updated: 2024/07/03 18:18:42 by sangyhan         ###   ########.fr       */
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
				std::cout << "check" << std::endl;
				// close(eventList[i].ident);
				continue;
			}
			if (eventList[i].flags & EV_EOF)
			{
				std::cerr << "***************** EV_EOF ENTERED *****************" << std::endl; 
				delete bufferList[j];
				bufferList.erase(bufferList.begin() + j);
				close(eventList[i].ident);
				struct kevent clientEvent;
				EV_SET(&clientEvent, eventList[i].ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
				changeList.push_back(clientEvent);
				std::cout << "EOF" << std::endl;
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
				std::cout << "----- server port : " << serverFd << " -----" << std::endl;
				readEvent(i, j, serverFd);
			}
			else if (eventList[i].filter == EVFILT_WRITE)
			{
				int serverFd = serverFdSet.find(eventList[i].ident)->second;
				std::cout << "----- server port : " << serverFd << " -----" << std::endl;
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
	// std::cout << "---- new client event ----" << std::endl;
	int clientFd = accept(serverFd, NULL, NULL);
	if (clientFd < 0) {
		throw RuntimeException("accept");
	} else {
		serverFdSet.insert(std::make_pair(clientFd, serverFd));
		setNonblock(clientFd);
		struct kevent clientEvent;
		EV_SET(&clientEvent, clientFd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		// std::cout << "---- new client added " << client_fd << " ----" << std::endl;
		changeList.push_back(clientEvent);

		Buffer* newClient = new Message(clientFd);
		bufferList.push_back(newClient);
		std::vector<Buffer*>::iterator iter = bufferList.begin();
		for (; iter != bufferList.end(); iter++) {
			if ((*iter)->getFd() == clientFd) {
				break;
			}
		}
		if (iter == bufferList.end()) {
			throw RuntimeException("client inset fail");
		}
	}
}

std::string Webserv::makeResponse() {
	std::string response;
	std::string content;

	std::ifstream file("./html/index.html", std::ios::in | std::ios::binary);
	if (file) {
		std::ostringstream file_buffer;
		file_buffer << file.rdbuf();
		content = file_buffer.str();

		response = "HTTP/1.1 200 OK\r\n";
		response += "Content-Length: " + std::to_string(content.length()) + "\r\n";
		response += "Content-Type: text/html\r\n\r\n";
		response += content;
	} else {
		content = "<h1>404 Not Found</h1>";
		response = "HTTP/1.1 404 Not Found\r\n";
		response += "Content-Length: " + std::to_string(content.length()) + "\r\n";
		response += "Content-Type: text/html\r\n\r\n";
		response += content;
	}
	return (response);
}

void Webserv::readEvent(int idx, int bufferIdx, int serverFd) {
	// std::cout << "****** read event ******" << std::endl;

	// EOF YES
	if (eventList[idx].flags & EV_EOF) {

		// Message == 1
		if (isMessage(bufferIdx) == 1) {

			// 소켓이 닫혔다는 거임 그거 버려야함
			closeSocket(bufferIdx);
			return ;
		}

		// File == 2
		else if (isMessage(bufferIdx) == 2) {

			// 여기는 파일 다 읽어서 클라이언트로 보낸다는거임
			closeFile(bufferIdx);

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
		// 파일을 덜 읽었다는 뜻임
		return ;
	}

	// Message == 1
	// 여기서 쭉쭉 내려가면 됨
	
	//std::cout << "buffer size : " << bufferList[bufferIdx].getReadBuffer().size() << std::endl;
	RequestParser parser;
<<<<<<< HEAD

	std::cout << "buffer size : " << bufferList[bufferIdx]->getReadBuffer().size() << std::endl;
=======
>>>>>>> 96092613a3aa9553f841c45c77c7753d66017939
	size_t endHeader;
	size_t endIndex = parser.checkEnd(bufferList[bufferIdx]->getReadBuffer(), buf, n, endHeader);
    if (endIndex != RequestParser::npos)
    {
        std::cout << "++++++++++++++++++++++++" << std::endl;
        std::cout << "+++++++ read end +++++++" << std::endl;
        std::cout << "++++++++++++++++++++++++" << std::endl;
        std::cout << std::endl;

<<<<<<< HEAD
		Buffer* buffer = bufferList[bufferIdx];
		llParser llparser(bufferList[bufferIdx]->getReadBuffer(), endHeader);
		HttpRequest request = llparser.parse();
		parser.setBody(request, bufferList[bufferIdx]->getReadBuffer(), endHeader, endIndex);

		serverList[serverFd].makeResponse(request, buffer);

        buffer->getReadBuffer().clear();
		
		struct kevent clientEvent;
		EV_SET(&clientEvent, clientFd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
		changeList.push_back(clientEvent);
=======
		Buffer buffer = bufferList[bufferIdx];
		llParser parser(bufferList[bufferIdx].getReadBuffer(), endHeader);
		try{
			HttpRequest request = parser.parse();
			serverList[serverFd].makeResponse(request, buffer);
       		struct kevent client_event;
			EV_SET(&client_event, client_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
			changeList.push_back(client_event);
		}
		catch (const std::runtime_error &e)
		{
			std::cerr << "Parsing error: " << e.what() << std::endl;
		}
		std::vector<char> temp =  std::vector<char>(buffer.getReadBuffer().begin() + endIndex + 4, buffer.getReadBuffer().end());
		buffer.getReadBuffer().clear();
		buffer.getReadBuffer() = temp;
>>>>>>> 96092613a3aa9553f841c45c77c7753d66017939
	}

}

void Webserv::writeEvent(int idx, int bufferIdx, int serverFd) {
	// std::cout << "****** write event ******" << std::endl;

	// std::cout << "HTTP REQUEST" << std::endl;
	// std::cout << bufferList[bufferIdx].getWriteBuffer().size() << std::endl;
	// for (int i = 0; i < bufferList[bufferIdx].getWriteBuffer().size(); i++) {
	// 	std::cout << bufferList[bufferIdx].getWriteBuffer()[i];
	// }


	// 일단 write하셈 ㅋㅋ
	std::string response(bufferList[bufferIdx]->getWriteBuffer().begin(), bufferList[bufferIdx]->getWriteBuffer().end());
	std::cout << "CHECK!!!!!\n\n" << response << std::endl;
	int writeSize = write(bufferList[bufferIdx]->getFd(), response.c_str(), response.length());
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

	// 여기서 사이즈가 서로 다르다는건 버퍼 사이즈 한계로 다 못썼다는 뜻 다시ㄱㄱ
	// 이터레이터로 어디까지 썼는지 알고 있어야 함
	if (writeSize != bufferList[bufferIdx]->getWriteBuffer().size()){
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
