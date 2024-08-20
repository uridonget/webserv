#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define ERROR -1
#define FOUND 0
#define NOT_FOUND 1

int check_directory_and_list_files(const char *path, const char *name) {
    DIR *dirp;
    struct dirent *dp;
    size_t len;

    dirp = opendir(path);
    if (dirp == NULL)
    {
        printf("Status: 400 Bad Request\r");
        printf("Content-Type: text/html; charset=utf-8\r\n\r");
        return (ERROR);
    }
    printf("Status: 200 OK\r");
    printf("Content-Type: text/html; charset=utf-8\r\n\r");
    len = strlen(name);
    while ((dp = readdir(dirp)) != NULL) {
        if (dp->d_type == DT_DIR && dp->d_namlen == len && strcmp(dp->d_name, name) == 0) {
            // 디렉토리 확인, HTML 출력 준비
            printf("<html><body>\n");
            printf("<h1>Directory listing for: %s</h1>\n", name);
            printf("<ul>\n");

            DIR *subdirp = opendir(dp->d_name);
            struct dirent *subdp;
            while ((subdp = readdir(subdirp)) != NULL) {
                if (strcmp(subdp->d_name, ".") != 0 && strcmp(subdp->d_name, "..") != 0) {
                    printf("<li>%s</li>\n", subdp->d_name);
                }
            }
            closedir(subdirp);

            printf("</ul>\n");
            printf("</body></html>\n");

            closedir(dirp);
            return (FOUND);
        }
    }
    closedir(dirp);
    return (NOT_FOUND);
}

int main(int argc, char **argv, char **envp) {
    const char *path = "./..";
    const char *name = envp[1]; // 원하는 디렉토리 이름으로 변경하세요

    int result = check_directory_and_list_files(path, name);
    if (result == ERROR) {
        fprintf(stderr, "Error opening directory.\n");
        return 1;
    } else if (result == NOT_FOUND) {
        fprintf(stderr, "Directory not found.\n");
        return 1;
    }

    return 0;
}