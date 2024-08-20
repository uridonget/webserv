/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 13:01:20 by haejeong          #+#    #+#             */
/*   Updated: 2024/08/20 15:39:37 by sangyhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Webserv.hpp"
#include "../include/Server.hpp"

Webserv::Webserv() : kq(0) {}

Webserv::~Webserv() {
	for (size_t i = 0; i < bufferList.size(); i++)
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

int Webserv::checkNewClient(unsigned long eventIdent) {
	std::map<int, Server>::iterator it = serverList.find(eventIdent);
	if (it != serverList.end()) {
		return eventIdent;
	}
	return -1;
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
		std::cout << "serecrFD : " << serverFd << std::endl;
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

void Webserv::readEvent(int idx, int bufferIdx, int serverFd) {
	if (eventList[idx].flags & EV_EOF) {
		if (isMessage(bufferIdx) == 1) {
			closeSocket(bufferIdx);
			return ;
		}
		// File == 2
		else if (isMessage(bufferIdx) == 2) {
			throw RuntimeException("read event");
			return ;
		}
	}
	int clientFd = eventList[idx].ident;
	char buf[BUFFER_SIZE] = {0};
	int n = read(clientFd, buf, BUFFER_SIZE);
	if (n < 0) {
		if (isMessage(bufferIdx) == 1) {
			closeSocket(bufferIdx);
			return ;
		}
		else if (isMessage(bufferIdx) == 2)
		{
			closeFile(bufferIdx);
			(static_cast<File *>(bufferList[bufferIdx]))->setError(true);
			std::map<int, Server>::iterator server = serverList.find(serverFd);
			if (server != serverList.end())
			{
       			struct kevent clientEvent;
				if (server->second.afterProcessRequest(bufferList[bufferIdx], clientEvent) == 1)
					changeList.push_back(clientEvent);
				serverFdMap.erase(bufferList[bufferIdx]->getFd());
			}
			delete static_cast<File *>(bufferList[bufferIdx]);
			bufferList.erase(bufferList.begin() + bufferIdx);
			return ;
		}
		else
		{
			Pipe *pipe = (static_cast<Pipe *>(bufferList[bufferIdx]));
			std::cout << "read error inputFd " << pipe->getInputFd() << std::endl;
			pipe->closeInput();
			pipe->closeOutput();
			pipe->setError(true);
			pipe->setReadEnd(true);
			if (pipe->End() == true)
			{
				std::map<int, Server>::iterator server = serverList.find(serverFd);
				if (server != serverList.end())
				{
					struct kevent clientEvent;
					if (server->second.afterProcessRequest(bufferList[bufferIdx], clientEvent) == 1)
						changeList.push_back(clientEvent);
					serverFdMap.erase(bufferList[bufferIdx]->getFd());
				}
				delete static_cast<Pipe *>(bufferList[bufferIdx]);
				bufferList.erase(bufferList.begin() + bufferIdx);
			}
			return ;
		}
	}
	if (isMessage(bufferIdx) == 2) // File
	{
		bufferList[bufferIdx]->getReadBuffer().insert(bufferList[bufferIdx]->getReadBuffer().end(), buf, buf + n);
		if (n != BUFFER_SIZE || n == 0)
		{
			closeFile(bufferIdx);
			std::map<int, Server>::iterator server = serverList.find(serverFd);
			if (server != serverList.end())
			{
       			struct kevent clientEvent;
				if (server->second.afterProcessRequest(bufferList[bufferIdx], clientEvent) == 1)
					changeList.push_back(clientEvent);
				serverFdMap.erase(bufferList[bufferIdx]->getFd());
				delete static_cast<File *>(bufferList[bufferIdx]);
				bufferList.erase(bufferList.begin() + bufferIdx);
			}
		}
		return ;
	}
	if (isMessage(bufferIdx) == 3) // Pipe
	{
		bufferList[bufferIdx]->getReadBuffer().insert(bufferList[bufferIdx]->getReadBuffer().end(), buf, buf + n);
		if (n == 0)
		{
			
			Pipe *pipe = (static_cast<Pipe *>(bufferList[bufferIdx]));
			pipe->setReadEnd(true);
			pipe->closeOutput();
			if (pipe->End() == true)
			{
				std::map<int, Server>::iterator server = serverList.find(serverFd);
				if (server != serverList.end())
				{
					struct kevent clientEvent;
					if (server->second.afterProcessRequest(bufferList[bufferIdx], clientEvent) == 1)
						changeList.push_back(clientEvent);
					serverFdMap.erase(bufferList[bufferIdx]->getFd());
					delete static_cast<Pipe *>(bufferList[bufferIdx]);
					bufferList.erase(bufferList.begin() + bufferIdx);
				}
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
			HttpRequest request = llparser.parse();
			request.bodyStart = endHeader + 4;
			request.bodyEnd = endIndex + 4;
			if (request.headers.find("Connection") != request.headers.end()) {
				if (request.headers["Connection"] == "close") {
					message->setStatus(WILL_BE_CLOSE);
				}
			}
			message->setContentLength(RequestParser::npos);
			std::map<int, Server>::iterator server = serverList.find(serverFd);
			if (server != serverList.end())
			{
				std::vector<struct kevent> clientEvent;
				std::vector<Buffer *> file;
				file = server->second.processRequest(bufferList[bufferIdx], request, clientEvent);
				for (size_t i = 0; i < file.size(); i++)
				{
					bufferList.push_back(file[i]);
					serverFdMap[file[i]->getFd()] = serverFd;
				}
				changeList.insert(changeList.end(), clientEvent.begin(), clientEvent.end());
			}
		}
		catch (const std::runtime_error &e)
		{
			std::string out;
			out += "HTTP/1.1 400 Bad Request\r\n";
			out += "\r\n";
			buffer->getWriteBuffer().reserve(out.size());
			buffer->getWriteBuffer().insert(buffer->getWriteBuffer().end(), out.begin(), out.end());
			pushEvent(buffer->getFd(), EVFILT_READ, EV_ADD | EV_ENABLE, changeList);
		}
		buffer->getReadBuffer().erase(buffer->getReadBuffer().begin(), buffer->getReadBuffer().begin() + endIndex + 4);
	}
}

void Webserv::writeEvent(int idx, int bufferIdx, int serverFd) {
	(void)idx;

	if (eventList[idx].flags & EV_EOF) {
		if (isMessage(bufferIdx) == 1) {
			closeSocket(bufferIdx);
			return ;
		}
	}
	int writeSize = BUFFER_SIZE;
	int writtenSize = bufferList[bufferIdx]->autoWrite(writeSize);
	if (writtenSize == -1)
	{
		if (isMessage(bufferIdx) == 1) {
			closeSocket(bufferIdx);
			return ;
		}
		else if (isMessage(bufferIdx) == 2)
		{
			File *file = static_cast<File *>(bufferList[bufferIdx]);
			file->getWriteBuffer().clear();
			file->setError(true);
			std::map<int, Server>::iterator server = serverList.find(serverFd);  
			struct kevent clientEvent;
			if (server->second.afterProcessRequest(bufferList[bufferIdx], clientEvent) == 1)
				changeList.push_back(clientEvent);
			successFileWrite(bufferIdx);
			return ;
		}
		else
		{
			Pipe *pipe = (static_cast<Pipe *>(bufferList[bufferIdx]));
			std::cout << "write event inputfd " << pipe->getInputFd() << std::endl;
			pipe->closeInput();
			pipe->setError(true);
			pipe->setReadEnd(true);
			if (pipe->End() == true)
			{
				std::map<int, Server>::iterator server = serverList.find(serverFd);
				if (server != serverList.end())
				{
					struct kevent clientEvent;
					if (server->second.afterProcessRequest(bufferList[bufferIdx], clientEvent) == 1)
						changeList.push_back(clientEvent);
					serverFdMap.erase(bufferList[bufferIdx]->getFd());
				}
				delete static_cast<Pipe *>(bufferList[bufferIdx]);
				bufferList.erase(bufferList.begin() + bufferIdx);
			}
			return ;
		}
	}
	if (!bufferList[bufferIdx]->writeBufEmpty()){
		return ;
	}
	// Message == 1
	if (isMessage(bufferIdx) == 1) { // 메시지 전송 성공
		bufferList[bufferIdx]->getWriteBuffer().clear();
		Message *message = static_cast<Message *>(bufferList[bufferIdx]);
        if (message->getStatus() == CLOSE)
        {
			closeSocket(bufferIdx);
        }
		else
		{
			successResponse(bufferIdx);
		}
		return ;
	}
	if (isMessage(bufferIdx) == 2) { // 파일 전송 성공
		bufferList[bufferIdx]->getWriteBuffer().clear();
		std::map<int, Server>::iterator server = serverList.find(serverFd);  
		struct kevent clientEvent;
		if (server->second.afterProcessRequest(bufferList[bufferIdx], clientEvent) == 1)
			changeList.push_back(clientEvent);
		successFileWrite(bufferIdx);
		return ;
	}
	if (isMessage(bufferIdx) == 3) { // 파일 전송 성공
		bufferList[bufferIdx]->getWriteBuffer().clear();
		Pipe *pipe = (static_cast<Pipe *>(bufferList[bufferIdx]));
		std::cout << "write sucess inputfd "  << pipe->getInputFd() << std::endl;
		pipe->closeInput();
		return ;
	}
}

void Webserv::runServers() {
	struct timespec timeout;
	timeout.tv_sec = 0;
	timeout.tv_nsec = 10000;

	signal(SIGPIPE, SIG_IGN);
	while (true) {
		int nev = kevent(kq, &changeList[0], changeList.size(), eventList, 1000, &timeout);
		if (nev < 0) {
			throw RuntimeException("kevent");
		}
		clock_t finish;
		double duration;
		for (std::map<int, Server>::iterator it = serverList.begin(); it != serverList.end(); ++it) {
			std::vector<pid_t> pidList;
			for (std::map<pid_t, clock_t>::iterator it2 = (*it).second.getChildTime().begin(); it2 != (*it).second.getChildTime().end(); ++it2) {
				finish = clock();
				duration = (double)(finish - (*it2).second) / CLOCKS_PER_SEC;
				if (duration > 10) {
					kill(it2->first, SIGKILL);
					pidList.push_back(it2->first);
				}
			}
			for (size_t i = 0; i < pidList.size(); i++)
			{
				it->second.getChildTime().erase(pidList[i]);
			}
		}
		changeList.clear();
		if (nev == 0) {
			continue;
		}
		for (int i=0; i < nev; i++) {
			if (checkSocketError(i)) {
				continue;
			}
			if (eventList[i].flags & EV_DELETE) {
				close(eventList->ident);
				continue;
			}
			if (eventList[i].filter == EVFILT_PROC && eventList[i].fflags == NOTE_EXIT) {
				int status;
				if (waitpid(eventList[i].ident, &status,0) == -1) {
					throw RuntimeException("waitpid");
				}
				for (size_t bufIndex = 0; bufIndex < bufferList.size(); bufIndex++)
				{
					if ((bufferList[bufIndex]) == static_cast<Buffer *>(eventList[i].udata))
					{
						if (WIFSIGNALED(status)) {
							(static_cast<Pipe *>(bufferList[bufIndex]))->setError(true);
						}
						Pipe *pipe = (static_cast<Pipe *>(bufferList[bufIndex]));
						pipe->setProcEnd(true);
						int serverFd = serverFdMap.find(pipe->getFd())->second;
						if (pipe->End() == true || WIFSIGNALED(status))
						{
							std::cout << "proc end out fd close " << pipe->getFd() << std::endl;
							pipe->closeOutput();
							pipe->closeInput();
							std::map<int, Server>::iterator server = serverList.find(serverFd);
							if (server != serverList.end())
							{
								struct kevent clientEvent;
								if (server->second.afterProcessRequest(bufferList[bufIndex], clientEvent) == 1)
									changeList.push_back(clientEvent);
								serverFdMap.erase(bufferList[bufIndex]->getFd());
								delete static_cast<Pipe *>(bufferList[bufIndex]);
								bufferList.erase(bufferList.begin() + bufIndex);
							}
						}
						break;
					}
				} 
				continue ;
			}
			int isNew = checkNewClient(eventList[i].ident);
			if (isNew > 0) {
				newClient(eventList[i].ident);
				continue ;
			}
			size_t j = 0;
			for (; j < bufferList.size(); j++) {
				if (bufferList[j]->whoAmI() == 3) {
					if (static_cast<Pipe*>(bufferList[j])->getInputFd() == static_cast<int>(eventList[i].ident)) {
						break;
					}
				}
				if (bufferList[j]->getFd() == static_cast<int>(eventList[i].ident)) {
					break;
				}
			}
			if (j == bufferList.size())
			{
				continue ;
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
