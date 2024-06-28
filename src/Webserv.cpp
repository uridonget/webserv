/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 13:01:20 by haejeong          #+#    #+#             */
/*   Updated: 2024/06/28 20:02:43 by sangyhan         ###   ########.fr       */
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
		serverFdSet.insert(serv.getServerFd());
		serverList.insert(std::make_pair(serv.getServerFd(), serv));
	}   
}

int Webserv::checkNewClient(uintptr_t eventIdent) {
	std::set<int>::iterator it = serverFdSet.find(eventIdent);
	if (it == serverFdSet.end()) { // server fd가 아니다! new client 아님!
		std::cout << "this is old client!" << std::endl;
		return -1;
	}
	else {
		std::cout << "this is new client!" << std::endl;
		return *it; // new client
	}
}

bool Webserv::checkSocketError(int idx, int bufferIdx) {
	if (eventList[idx].flags == EV_ERROR) {
		std::cerr << "***************** EV_ERROR ENTERED *****************" << std::endl; 
		if (checkNewClient(eventList[idx].ident)) {
			throw RuntimeException("Server socket");
		} else {
			// close(eventList[idx].ident);
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
		// if (cnt == 20)
		//	 std::exit(0);
		int nev = kevent(kq, &changeList[0], changeList.size(), eventList, 10, &timeout);
		std::cout << "NUMBER OF EVENT : " << nev << std::endl;
		if (nev < 0)
		{
			throw RuntimeException("kevent");
		}
		changeList.clear();
		if (nev == 0)
		{
			std::cout << "NO EVENT" << std::endl;
			continue;
		}
		for (int i = 0; i < nev; i++)
		{
			printf("\n----filter : %d fflags : %x flags : %x ----\n\n", eventList[i].filter, eventList[i].fflags, eventList[i].flags);

			int j = 0;
			for (; j < bufferList.size(); j++) {
				if (bufferList[j].getFd() == eventList[i].ident) {
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
				for (int i = 0; i < bufferList.size(); i++) {
					if (bufferList[i].getFd() == eventList[i].ident) {
						bufferList.erase(bufferList.begin() + i);
						break;
					}
				}
				// clients.erase(eventList[i].ident);
				close(eventList[i].ident);
				struct kevent client_event;
				EV_SET(&client_event, eventList[i].ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
				changeList.push_back(client_event);
				std::cout << "EOF" << std::endl;
				continue;
			}
			int temp = checkNewClient(eventList[i].ident);
			if (temp > 0)
			{
				new_client(eventList[i].ident); // new client
			}
			else if (eventList[i].filter == EVFILT_READ)
			{
				readEvent(i, j);
			}
			else if (eventList[i].filter == EVFILT_WRITE)
			{
				writeEvent(i, j);
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

void Webserv::new_client(int serverFd) {
	std::cout << "---- new client event ----" << std::endl;
	int client_fd = accept(serverFd, NULL, NULL);
	if (client_fd < 0) {
		throw RuntimeException("accept");
	} else {
		setNonblock(client_fd);
		struct kevent client_event;
		EV_SET(&client_event, client_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		std::cout << "---- new client added " << client_fd << " ----" << std::endl;
		changeList.push_back(client_event);

		Message newClient(client_fd);
		bufferList.push_back(newClient);
		std::vector<Buffer>::iterator iter = bufferList.begin();
		for (; iter != bufferList.end(); iter++) {
			if ((*iter).getFd() == client_fd) {
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

void Webserv::readEvent(int idx, int bufferIdx) {
	std::cout << "****** read event ******" << std::endl;

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
			close(bufferIdx);

			return ;
		}
	}

	// EOF NO
	int client_fd = eventList[idx].ident; 

	char buf[BUFFER_SIZE] = {0};
	int n = read(client_fd, buf, BUFFER_SIZE);
	// 예외처리
	if (n == -1)
	{
		bufferList.erase(bufferList.begin() + bufferIdx);
		close(client_fd);
		struct kevent client_event;
		EV_SET(&client_event, client_fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
		changeList.push_back(client_event);
		return ;
	}

	// File == 2
	if (isMessage(bufferIdx) == 2) {
		// 파일을 덜 읽었다는 뜻임
		return ;
	}

	// Message == 1
	// 여기서 쭉쭉 내려가면 됨


	RequestParser parser;

    std::cout << "buffer size : " << bufferList[bufferIdx].getBuffer().size() << std::endl;
	size_t endHeader;
	size_t endIndex = parser.checkEnd(bufferList[bufferIdx].getBuffer(), buf, n, endHeader);
    if (endIndex != RequestParser::npos)
    {
        std::cout << "++++++++++++++++++++++++" << std::endl;
        std::cout << "+++++++ read end +++++++" << std::endl;
        std::cout << "++++++++++++++++++++++++" << std::endl;
        std::cout << std::endl;
        bufferList[bufferIdx].getBuffer().clear();
		struct kevent client_event;

        EV_SET(&client_event, client_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
        changeList.push_back(client_event);

    }

}

void Webserv::writeEvent(int idx, int bufferIdx) {
	std::cout << "****** write event ******" << std::endl;


	int clientFd = eventList[idx].ident;
	std::cout << "HTTP REQUEST" << std::endl;
	std::cout << bufferList[bufferIdx].getBuffer().size() << std::endl;
	// for (int i=0; i < bufferList[bufferIdx].getBuffer().size(); i++) {
	// 	std::cout << bufferList[bufferIdx].getBuffer()[i];
	// }

	// Message == 1
	if (isMessage(bufferIdx) == 1) {

		// 여기 뭔지 모르겠음;;
		// struct kevent clientEvent;
		// EV_SET(&clientEvent, clientFd, EVFILT_WRITE, EV_DELETE , 0, 0, NULL);
		// changeList.push_back(clientEvent);
		return ;
	}

	// File == 2

	std::string response = makeResponse();
	int n = write(clientFd, response.c_str(), response.length());
	if (n == -1)
	{
		if (close(clientFd) == -1)
			std::cout << "already closed fd!!!" << std::endl;
		bufferList.erase(bufferList.begin() + bufferIdx);
		struct kevent client_event;
		struct kevent client_event2;
		EV_SET(&client_event2, clientFd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
		EV_SET(&client_event, clientFd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
		changeList.push_back(client_event);
		changeList.push_back(client_event2);
		return ;
	}
	struct kevent client_event1;
	EV_SET(&client_event1, clientFd, EVFILT_WRITE, EV_DELETE , 0, 0, NULL);
	changeList.push_back(client_event1);
}

void Webserv::connectKqueueToServer() {
	for (size_t i = 0; i < serverList.size(); i++) {
		struct kevent ke;
		EV_SET(&ke, serverList[i].getServerFd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		changeList.push_back(ke);
	}
}
