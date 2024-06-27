/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 13:01:20 by haejeong          #+#    #+#             */
/*   Updated: 2024/06/27 16:50:05 by haejeong         ###   ########.fr       */
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

void Webserv::makeServerConfigStringList(const std::string &configPath)
{
    std::ifstream configFile(configPath.c_str());
    if (!configFile.is_open())
    {
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

std::vector<ServerConfig> Webserv::getServerConfigs()
{
    return (this->configParsing.getServerConfigs());
}

void Webserv::makeServerList()
{
    std::vector<ServerConfig> configs;

    configs = this->configParsing.getServerConfigs();
    for (std::vector<ServerConfig>::iterator it = configs.begin(); it != configs.end(); it++)
    {
        Server serv;
        serv.initServer(*it);
        serverFdSet.insert(serv.getServerFd());
        serverList.insert(std::make_pair(serv.getServerFd(), serv));
    }
}

int Webserv::checkNewClient(uintptr_t eventIdent)
{
    std::set<int>::iterator it = serverFdSet.find(eventIdent);
    if (it == serverFdSet.end())
    { // server fd가 아니다! new client 아님!
        std::cout << "this is old client!" << std::endl;
        return -1;
    }
    else
    {
        std::cout << "this is new client!" << std::endl;
        return *it; // new client
    }
}

bool Webserv::checkSocketError(int idx)
{
    if (eventList[idx].flags == EV_ERROR)
    {
        if (checkNewClient(eventList[idx].ident))
        {
            throw RuntimeException("Server socket");
        }
        else
        {
            close(eventList[idx].ident);
            clients.erase(eventList[idx].ident);
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
        //     std::exit(0);
        int nev = kevent(kq, &changeList[0], changeList.size(), eventList, 10, &timeout);
        std::cout << "NUMBER OF EVENT : " << nev << std::endl;
        if (nev < 0)
        {
            perror("kqueue");
            throw RuntimeException("kqueue1");
        }
        changeList.clear();
        if (nev == 0)
        {
            std::cout << "NO EVENT" << std::endl;
            continue;
        }
        for (int i = 0; i < nev; i++)
        {
            printf("\n----filter : %d fflags : %d flags : %d ----\n\n", eventList[i].filter, eventList[i].fflags, eventList[i].flags);
            if (checkSocketError(i))
                continue; // error check
            if (eventList[i].flags & EV_DELETE)
            {
                std::cout << "check" << std::endl;
                continue;
            }
            if (eventList[i].flags & EV_EOF)
            {
                clients.erase(eventList[i].ident);
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
                read_event(i);
            }
            else if (eventList[i].filter == EVFILT_WRITE)
            {
                write_event(i);
            }
        }
    }
}

void Webserv::initKqueue()
{
    kq = kqueue();
    if (kq < 0)
    {
        throw RuntimeException("kqueue2");
    }
}

static void set_nonblock(int fd)
{
    if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
    {
        perror("fcntl(F_SETFL)");
        std::exit(EXIT_FAILURE);
    }
}

void Webserv::new_client(int serverFd)
{
    std::cout << "---- new client event ----" << std::endl;
    int client_fd = accept(serverFd, NULL, NULL);
    if (client_fd < 0)
    {
        std::cout << "serverFd : " << serverFd << std::endl;
        perror("accept");
        exit(1);
    }
    else
    {
        set_nonblock(client_fd);
        struct kevent client_event;
        EV_SET(&client_event, client_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
        std::cout << "---- new client added " << client_fd << " ----" << std::endl;
        changeList.push_back(client_event);
        clients[client_fd] = std::vector<char>();
        if (clients.find(client_fd) == clients.end())
            exit(1);
    }
}

std::string Webserv::make_response()
{
    std::string response;
    std::string content;

    std::ifstream file("./html/index.html", std::ios::in | std::ios::binary);
    if (file)
    {
        std::ostringstream file_buffer;
        file_buffer << file.rdbuf();
        content = file_buffer.str();

        response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Length: " + std::to_string(content.length()) + "\r\n";
        response += "Content-Type: text/html\r\n\r\n";
        response += content;
    }
    else
    {
        content = "<h1>404 Not Found</h1>";
        response = "HTTP/1.1 404 Not Found\r\n";
        response += "Content-Length: " + std::to_string(content.length()) + "\r\n";
        response += "Content-Type: text/html\r\n\r\n";
        response += content;
    }
    return (response);
}

void Webserv::read_event(int idx)
{
    std::cout << "****** read event ******" << std::endl;
    if (eventList[idx].flags & EV_DELETE)
        std::cout << "hoxy?" << std::endl;
    int client_fd = eventList[idx].ident;
    std::map<int, std::vector<char> >::iterator it = clients.find(client_fd);
    if (it == clients.end())
    {
        std::cout << "client not exist in server!!!" << std::endl;
        return;
    }
    char buf[BUFFER_SIZE] = {0};
    int n = read(client_fd, buf, BUFFER_SIZE);
    if (n == -1)
    {
        clients.erase(client_fd);
        close(client_fd);
        struct kevent client_event;
        EV_SET(&client_event, client_fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
        changeList.push_back(client_event);
        return;
    }
    it->second.insert(it->second.end(), buf, buf + n);
    std::cout << "buffer size : " << it->second.size() << std::endl;
    if (n != BUFFER_SIZE)
    {
        std::cout << "++++++++++++++++++++++++" << std::endl;
        std::cout << "+++++++ read end +++++++" << std::endl;
        std::cout << "++++++++++++++++++++++++" << std::endl;
        std::cout << std::endl;
        struct kevent client_event;
        EV_SET(&client_event, client_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
        changeList.push_back(client_event);
    }
}

void Webserv::write_event(int idx)
{
    // if (eventList[idx].flags == EV_DELETE) {
    //     // close(eventList[idx].ident);
    //     // clients.erase(eventList[idx].ident);
    //     // serverFdSet.erase(eventList[idx].ident);
    //     std::cout << "여기입니다!" << std::endl;
    //     return ;
    // }
    std::cout << "****** write event ******" << std::endl;
    int client_fd = eventList[idx].ident;
    std::map<int, std::vector<char> >::iterator it = clients.find(client_fd);
    if (it == clients.end())
    {
        std::cout << "client not exist in server!!!" << std::endl;
        return;
    }
    std::cout << "HTTP REQUEST" << std::endl;
    std::cout << it->second.size() << std::endl;
    for (int i = 0; i < it->second.size(); i++)
    {
        std::cout << it->second[i];
    }
    std::string response = make_response();
    int n = write(client_fd, response.c_str(), response.length());
    if (n == -1)
    {
        clients.erase(client_fd);
        close(client_fd);
        struct kevent client_event;
        // struct kevent client_event2;
        // EV_SET(&client_event2, client_fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
        EV_SET(&client_event, client_fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
        changeList.push_back(client_event);
        // changeList.push_back(client_event2);
        return;
    }
    it->second.clear();
    struct kevent client_event1;
    EV_SET(&client_event1, client_fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    changeList.push_back(client_event1);
}

void Webserv::connectKqueueToServer()
{
    for (size_t i = 0; i < serverList.size(); i++)
    {
        struct kevent ke;
        EV_SET(&ke, serverList[i].getServerFd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
        changeList.push_back(ke);
    }
}