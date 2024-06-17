#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080

void handleClient(int client_socket) {
    const int buffer_size = 1024;
    char buffer[buffer_size] = {0};
    read(client_socket, buffer, buffer_size);

    std::cout << "Received request:\n" << buffer << std::endl;

    std::string response;
    std::string content;

    // 파일 읽기
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

    send(client_socket, response.c_str(), response.length(), 0);
    close(client_socket);
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // 소켓 생성
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 소켓 옵션 설정: SO_REUSEADDR
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 주소와 포트 설정
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // 모든 인터페이스에서 수신 대기
    address.sin_port = htons(PORT);

    // 소켓을 주소에 바인딩
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 연결 요청 대기
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "HTTP server is running on port " << PORT << std::endl;

    while (true) {
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            close(server_fd);
            exit(EXIT_FAILURE);
        }
        handleClient(client_socket);
    }

    close(server_fd);
    return 0;
}
