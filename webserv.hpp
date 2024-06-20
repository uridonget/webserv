#ifndef WEBSERV
# define WEBSERV

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <fcntl.h>
#include <algorithm>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>

#define PORT 8080
#define BUFFER_SIZE 1024

class webserv {
	private:
		int server_fd; // server socket
		int kq; // kq
		std::map<int, std::vector<char> > client; // <client socket fd, buffer>
		std::vector<struct kevent> changeList; // kevents waiting for kqueue
		struct kevent events[10]; // events waiting for process

	public:
		webserv();
		~webserv();
		void set_nonblock(int fd);
		void init_server(void);
		void init_kqueue(void);
		void handle_event(int nevents);
		bool check_socket_error(int idx);
		void new_client();
		std::string make_response(std::vector<char*> envp);
		void read_event(int idx);
		void write_event(int idx);
		void run_server(void);

		std::map<std::string, std::string> parseRequest(const std::string & request);
		std::vector<char*> createEnvp(const std::map<std::string, std::string> &env_vars);
};

#endif