/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 14:57:20 by haejeong          #+#    #+#             */
/*   Updated: 2024/06/19 16:07:58 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

webserv::webserv() {
    std::cout << "webserv constructor called" << std::endl;
}

webserv::~webserv() {
    std::cout << "webserv destructor called" << std::endl;
}

void webserv::set_nonblock(int fd) {
    if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
        perror("fcntl(F_SETFL)");
        std::exit(EXIT_FAILURE);
    }
}

void webserv::init_server() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        std::exit(EXIT_FAILURE);
    }
    
    set_nonblock(server_fd);
    
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind");
        std::exit(EXIT_FAILURE);
    }
    
    if (listen(server_fd, 10) < 0) {
        perror("listen");
        std::exit(EXIT_FAILURE);
    }
}

void webserv::init_kqueue() {
    kq = kqueue();
    if (kq < 0) {
        perror("kqueue");
        std::exit(EXIT_FAILURE);
    }
    struct kevent check;
    EV_SET(&check, server_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    if (kevent(kq, &check, 1, NULL, 0, NULL) < 0) {
        perror("kevent");
        std::exit(EXIT_FAILURE);
    }
}

bool webserv::check_socket_error(int idx) {
    if (events[idx].flags == EV_ERROR) {
        if (events[idx].ident == server_fd) {
            perror("Server socket error");
            std::exit(EXIT_FAILURE);
        } else {
            perror("Client socket error");
            close(events[idx].ident);
            client.erase(events[idx].ident);
            return true;
        }
    }
    return false;
}

void webserv::new_client() {
    std::cout << "****** new client ******" << std::endl;
    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd < 0) {
        perror("accept");
    } else {
        set_nonblock(client_fd);
        struct kevent client_event;
        EV_SET(&client_event, client_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
        changeList.push_back(client_event);
        if (kevent(kq, &client_event, 1, NULL, 0, NULL) < 0) {
            perror("kevent");
            close(client_fd);
        }
        client[client_fd] = std::vector<char>();
    }
}

std::string webserv::make_response() {
    std::string response;
    std::string content;

    std::ifstream file("index.html", std::ios::in | std::ios::binary);
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

void webserv::run_server() {
    struct timespec timeout;
    timeout.tv_sec = 5;
    timeout.tv_nsec = 0;
    while (true) {
        int nev = kevent(kq, &changeList[0], changeList.size(), events, 10, &timeout);
        if (nev < 0) {
            perror("kevent");
            std::exit(EXIT_FAILURE);
        }
        else if (nev == 0) {
            continue ;
        }
        changeList.clear();
        std::cout << "NUMBER OF EVENT : " << nev << std::endl;
        for (int i=0; i < nev; i++) {
            if (check_socket_error(i))
                continue ; // error check
            if (events[i].ident == server_fd) {
                new_client(); // new client
            } else if (events[i].filter == EVFILT_READ) {
                std::cout << "****** read event ******" << std::endl;
                int client_fd = events[i].ident;
                std::map<int, std::vector<char> >::iterator it = client.find(client_fd);
                char buf[BUFFER_SIZE] = {0};
                int n = read(client_fd, buf, BUFFER_SIZE);
                for (int i=0; i < n; i++) {
                    it->second.push_back(buf[i]);
                }
                if (n != BUFFER_SIZE) {
                    std::cout << "****** read end ******" << std::endl;
                    struct kevent client_event;
                    EV_SET(&client_event, client_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
                    changeList.push_back(client_event);
                }
            } else if (events[i].filter == EVFILT_WRITE) {
                std::cout << "****** write event ******" << std::endl;
                int client_fd = events[i].ident;
                std::map<int, std::vector<char> >::iterator it = client.find(client_fd);
                for (int i=0; i < it->second.size(); i++) {
                    std::cout << it->second[i];
                }
                std::string response = make_response();
                write(client_fd, response.c_str(), response.length());
                it->second.clear();
                struct kevent client_event;
                EV_SET(&client_event, client_fd, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL);
                changeList.push_back(client_event);
            }
        }
    }
    close(server_fd);
}

int main() {

    webserv server;

    server.init_server();
    server.init_kqueue();
    server.run_server();
}
