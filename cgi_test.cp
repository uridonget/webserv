#include <iostream>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>

int main() {

    char *const envp[] = {
        (char *)"AUTH_TYPE=",                   // 서블릿을 보호하는 데 사용되는 인증 스키마의 이름입니다. 예를 들면 BASIC, SSL 또는 서블릿이 보호되지 않는 경우 null입니다.
        (char *)"CONTENT_LENGTH=0",             // 입력 스트림에서 사용 가능한 요청 본문의 길이(바이트) 또는 길이를 알 수 없는 경우 -1입니다. HTTP 서블릿의 경우 리턴되는 값은 CGI 변수 CONTENT_LENGTH의 값과 동일합니다.
        (char *)"CONTENT_TYPE=text/html",       // 요청 본문의 MIME 유형 또는 유형을 모르는 경우 null입니다. HTTP 서블릿의 경우 리턴되는 값은 CGI 변수 CONTENT_TYPE의 값과 동일합니다
        (char *)"GATEWAY_INTERFACE=CGI/1.1",    // 서버가 스크립트와 통신하기 위해 사용하는 CGI 스펙의 버전입니다.
        (char *)"HTTP_ACCEPT=",                 // "HTTP_"로 시작하는 이름을 가진 변수는 사용되는 스키마가 HTTP인 경우 요청 헤더의 값을 포함합니다. HTTP_ACCEPT는 브라우저가 지원하는 내용 유형을 지정합니다. 예를 들어, text/xml입니다.
        (char *)"HTTP_ACCEPT_CHARSET=",         // 문자 환경 설정 정보. 정보가 있는 경우 클라이언트의 선호하는 문자 세트를 표시하는 데 사용됩니다. 예를 들어, utf-8;q=0.5입니다.
        (char *)"HTTP_ACCEPT_ENCODINGE=",       // 클라이언트로 리턴되는 내용에 대해 수행될 수 있는 인코딩의 유형을 정의합니다. 예를 들어, compress;q=0.5입니다.
        (char *)"HTTP_ACCEPT_LANGUAGE=",        // 수신할 내용에 적합한 언어를 정의하는 데 사용됩니다. 예를 들어, en;q=0.5입니다. 리턴되는 결과가 없으면 언어 환경 설정이 표시되지 않습니다.
        (char *)"HTTP_FORWARDED=",              // 요청이 전달되는 경우 프록시 서버의 주소 및 포트를 표시합니다.
        (char *)"HTTP_HOST=",                   // 요청되는 자원의 인터넷 호스트 및 포트 번호를 지정합니다. 모든 HTTP/1.1 요청의 경우 필수입니다.
        (char *)"HTTP_PROXY_AUTHORIZATION=",    // 인증이 필요한 프록시에 클라이언트 자체 또는 클라이언트의 사용자를 식별하기 위해 클라이언트가 사용합니다.
        (char *)"HTTP_USER_AGENT=",             // 클라이언트가 요청을 보내기 위해 사용 중인 브라우저의 유형 및 버전입니다. 예를 들어, Mozilla/1.5입니다.
        (char *)"PATH_INFO=/hello.html",        // 선택적으로 스크립트를 호출한 HTTP 요청의 추가 경로 정보를 포함하고 있으며 CGI 스크립트로 해석할 경로를 지정합니다. PATH_INFO는 CGI 스크립트가 리턴할 자원 또는 하위 자원을 식별하며 스크립트 이름 뒤에 오지만 모든 조회 데이터 앞에 오는 URI 경로 부분에서 파생됩니다.
        (char *)"PATH_TRANSLATED=",             // 스크립트의 가상 경로를 스크립트를 호출하는 데 사용되는 실제 경로에 맵핑합니다. 맵핑은 요청 URI의 PATH_INFO 구성요소를 가져와 적합한 가상 대 실제 변환을 수행하여 실행됩니다.
        (char *)"QUERY_STRING=",                // 경로 뒤의 요청 URL에 포함된 조회 문자열입니다.
        (char *)"REMOTE_ADDR=",                 // 요청을 보낸 클라이언트의 IP 주소를 리턴합니다. HTTP 서블릿의 경우 리턴되는 값은 CGI 변수 REMOTE_ADDR의 값과 동일합니다.
        (char *)"REMOTE_HOST=",                 // 요청을 보낸 클라이언트의 완전한 이름이거나 이름을 판별할 수 없는 경우 클라이언트의 IP 주소입니다. HTTP 서블릿의 경우 리턴되는 값은 CGI 변수 REMOTE_HOST의 값과 동일합니다.
        (char *)"REMOTE_USER=",                 // 사용자가 인증된 경우 이 요청을 작성한 사용자의 로그인을 리턴하고 사용자가 인증되지 않은 경우 널(null)을 리턴합니다.
        (char *)"REQUEST_METHOD=GET",           // 이 요청을 작성할 때 사용된 HTTP 메소드의 이름을 리턴합니다. 예를 들어, GET, POST 또는 PUT입니다.
        (char *)"SCRIPT_NAME=",                 // 프로토콜 이름에서 HTTP 요청의 첫 번째 라인에 있는 조회 문자열까지, URL의 부분을 리턴합니다.
        (char *)"SERVER_NAME=localhost",        // 요청을 수신한 서버의 호스트 이름을 리턴합니다. HTTP 서블릿의 경우 CGI 변수 SERVER_NAME의 값과 동일합니다.
        (char *)"SERVER_PORT=80",               // 이 요청이 수신된 포트 번호를 리턴합니다. HTTP 서블릿의 경우 리턴되는 값은 CGI 변수 SERVER_PORT의 값과 동일합니다.
        (char *)"SERVER_PROTOCOL=HTTP/1.1",     // 요청이 사용하는 프로토콜의 이름과 버전을 protocol/majorVersion.minorVersion 양식으로 리턴합니다. 예를 들어, HTTP/1.1입니다. HTTP 서블릿의 경우 리턴되는 값은 CGI 변수 SERVER_PROTOCOL의 값과 동일합니다.
        (char *)"SERVER_SOFTWARE=versbew",      // 서블릿이 실행 중인 서블릿 컨테이너의 이름과 버전을 리턴합니다.
        (char *)"HTTP_COOKIE=",                 // HTTP 쿠키 문자열
        (char *)"WEBTOP_USER=",                 // 로그인한 사용자의 사용자 이름
        (char *)"NCHOME=",                      // NCHOME 환경 변수
        NULL
    };


    // 파이프를 위한 파일 디스크립터
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    // 새로운 프로세스 생성
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }
    
    // 자식 프로세스
    if (pid == 0) {

        // 파이프를 통해 표준 입력으로 데이터 전달
        if (dup2(pipefd[0], STDIN_FILENO) == -1) {
            perror("dup2");
            return 1;
        }
        close(pipefd[0]);
        close(pipefd[1]);



        std::string file_name = "./cgi_test.py"; 
        std::vector<char *> exec_args;
        exec_args.push_back(const_cast<char *>(file_name.c_str()));
        exec_args.push_back(NULL);
        execve(file_name.c_str(), exec_args.data(), envp);
        perror("execve");
        return 1;

    } else { // 부모 프로세스
        close(pipefd[0]);

        std::string input_data = "input_data=webserv too hard....\n";
        write(pipefd[1], input_data.c_str(), input_data.length());
        close(pipefd[1]);

        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            std::cout << "Child process exited with status " << WEXITSTATUS(status) << std::endl;
        } else {
            std::cerr << "Child process did not exit normally" << std::endl;
        }
    }

    return 0;
}

