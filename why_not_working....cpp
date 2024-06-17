#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <fcntl.h>
#include <algorithm>
#include <map>
#include <vector>

const int PORT = 8080;
const int BUFFER_SIZE = 10;
const char *RESPONSE =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/plain\r\n"
    "Content-Length: 13\r\n"
    "\r\n"
    "Hello, World!";

void set_nonblocking(int sockfd)
{
    fcntl(sockfd, F_SETFL, O_NONBLOCK);
}

int init_server(int server_fd)
{
    int opt = 1;
    struct sockaddr_in address;

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) == -1)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    set_nonblocking(server_fd);
    int kq = kqueue();
    if (kq == -1)
    {
        perror("kqueue");
        exit(EXIT_FAILURE);
    }
    return kq;
}

int main()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    std::map<int, std::string> client;
    std::vector<struct kevent> change_list;

    if (server_fd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    int kq = init_server(server_fd);
    struct kevent change;
    EV_SET(&change, server_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    if (kevent(kq, &change, 1, NULL, 0, NULL) == -1)
    {
        perror("kevent");
        exit(EXIT_FAILURE);
    }
    change_list.push_back(change);
    struct kevent events[10];
    while (true)
    {
        int nevents = kevent(kq, &change_list[0], change_list.size(), events, 10, NULL);
        if (nevents == -1)
        {
            perror("kevent");
            exit(EXIT_FAILURE);
        }
        change_list.clear();
        for (int i = 0; i < nevents; ++i)
        {
            if (events[i].flags & EV_ERROR)
            {
                if (events[i].ident == server_fd)
                {
                    perror("server socket error");
                    exit(EXIT_FAILURE);
                }
                else
                {
                    std::cerr << "client socket error" << std::endl;
                    std::cout << "client disconnected: " << events[i].ident << std::endl;
                    close(events[i].ident);
                    client.erase(events[i].ident);
                }
            }
            else if (events[i].filter == EVFILT_READ)
            {
                if (events[i].ident == server_fd)
                {
                    int client_fd = accept(server_fd, NULL, NULL);
                    if (client_fd == -1)
                    {
                        perror("accept");
                        continue;
                    }
                    set_nonblocking(client_fd);
                    struct kevent client_event1;
                    struct kevent client_event2;
                    EV_SET(&client_event1, client_fd, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL);
                    EV_SET(&client_event2, client_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                    change_list.push_back(client_event1);
                    change_list.push_back(client_event2);
                    std::string buf = "";
                    client[client_fd] = buf;
                }
                else
                {
                    int client_fd = events[i].ident;
                    std::map<int, std::string>::iterator it = client.find(client_fd);
                    char buf[100];
                    int n = read(events[i].ident, buf, sizeof(buf));
                    it->second += buf;
                    if (n < sizeof(buf))
                    {
                            struct kevent client_event;
                            EV_SET(&client_event, client_fd, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
                            change_list.push_back(client_event);
                    }
                }
            }
            else if (events[i].filter == EVFILT_WRITE)
            {
                int client_fd = events[i].ident;
                std::map<int, std::string>::iterator it = client.find(client_fd);
                std::cout << it->second << std::endl;
                write(client_fd, RESPONSE, strlen(RESPONSE));
                struct kevent client_event;
                EV_SET(&client_event, client_fd, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);
                change_list.push_back(client_event);
            }
        }
    }
    close(server_fd);
    return 0;
}
