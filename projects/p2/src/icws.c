#include <stdio.h>
#include <stdlib.h>
#include "pcsa_net.h"
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<getopt.h>
#include<time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "parse.h"
#include <pthread.h>

#define MAXBUF 8192

char port[MAXBUF];
char numberPort[MAXBUF];
char path2[MAXBUF];
char globalUri[MAXBUF];
char path[MAXBUF];
char fileToOpen[MAXBUF];
int uriLength;
char mimeType[200];

typedef struct sockaddr SA;

void serve_http(int connFd);
void run();
void respondIndex(int connFd, char *method);
void* conn_handler(void *);

struct twoTime {
    struct sockaddr_storage clientAddr;
    int connFd;

}*sss;


char *find_Date(){

    char day[200];
    time_t clock;
    time(&clock);
    struct tm *machine = localtime(&clock);
    sprintf(day, "%d/%d/%d", machine->tm_mday, machine->tm_mon + 1, machine->tm_year + 1900);

    return strdup(day);

}

void getType(char *file) {
    memset(mimeType, 0, strlen(mimeType));

    if (strstr(file, "html")) {
        memcpy(mimeType, "text/html", strlen("text/html"));
    }
    if (strstr(file, "css")) {
        memcpy(mimeType, "text/css", strlen("text/css"));
    }
    if (strstr(file, "js")) {
        memcpy(mimeType, "text/javascript", strlen("text/javascript"));
    }
    if (strstr(file, "jpeg")) {
        memcpy(mimeType, "image/jpg", strlen("image/jpg"));
    }
    if (strstr(file, "png")) {
        memcpy(mimeType, "image/png", strlen("image/png"));
    }
    if (strstr(file, "gif")) {
        memcpy(mimeType, "image/gif", strlen("image/gif"));
    }
}

void respondIndex(int connFd, char *method) {
    struct stat freeStruct;
    struct stat ttt;

    uriLength = 0;
    memset(fileToOpen, 0, strlen(fileToOpen));
    printf("DEBUG URI:%s and length is %lu\n", globalUri, strlen(globalUri));
    memcpy(fileToOpen, path, strlen(path));
    if (!strcmp(globalUri, "/")) {
        uriLength = strlen("/index.html");
        memset(globalUri, 0, strlen(globalUri));
        memcpy(globalUri, "/index.html", uriLength);
    }
    uriLength = strlen(globalUri);
    strcat(fileToOpen, globalUri);
    if (stat(fileToOpen, &ttt) == -1) {
        perror("stat");

    }
    printf("File to open is %s with length %lu\n", fileToOpen, strlen(fileToOpen));
    int fd = open(fileToOpen, O_RDONLY);
    fstat(fd, &ttt);
    char buf[MAXBUF];
    ssize_t bytesRead;
    getType(fileToOpen);


    if (fd == -1) {
        fprintf(stderr, "ERROR opening\n");
        sprintf(buf, "HTTP/1.1 404 Not found\r\n"
                     "Server: ICWS\r\n"
                     "Date: %s \r\n"
                     "Connection: close\r\n",find_Date());

    } else {
        sprintf(buf, "HTTP/1.1 200 OK\r\n"
                     "Server: ICWS\r\n"
                     "Date: %s \r\n"
                     "Connection: close\r\n"
                     "Content-length: %lld\r\n"
                     "Content-type: %s\r\n"
                     "Last modified: %s \r\n\r\n",find_Date(), ttt.st_size, mimeType, ctime(&ttt.st_mtime));
    }

    if (!strcmp(method, "GET")) {
        write_all(connFd, buf, strlen(buf));
        while ((bytesRead = read(fd, buf, MAXBUF)) > 0) {
            ssize_t numToWrite = bytesRead;
            char *writeBuf = buf;
            while (numToWrite > 0) {
                ssize_t numWritten = write(connFd, writeBuf, numToWrite);
                if (numWritten < 0) {
                    printf("ERROR: Couldn't write\n");
                    break;
                }
                numToWrite -= numWritten;
                writeBuf += numWritten;
            }
        }
    }
    if (!strcmp(method, "HEAD")) {
        write_all(connFd, buf, strlen(buf));
    }
    close(fd);
    printf("File closed\n");
    ttt = freeStruct;
}

void serve_http(int connFd) {
    memset(globalUri, 0, uriLength);
    char buf[MAXBUF];
    int readRet = read(connFd, buf, MAXBUF);
    Request *request = parse(buf, readRet, connFd);

    printf("Http Method %s\n", request->http_method);
    printf("Http Version %s\n", request->http_version);
    printf("Http Uri %s\n", request->http_uri);
    memcpy(globalUri, request->http_uri, strlen(request->http_uri));
    printf("Success\n");
    if (strcasecmp(request->http_method, "GET") == 0) {
        // There is a request of the form GET / {SOME_VERSION}
        // We should respond
        respondIndex(connFd, "GET");
    }
    if (strcasecmp(request->http_method, "HEAD") == 0) {
        respondIndex(connFd, "HEAD");
    }
    free(request->headers);
    free(request);
}


void run() {
    int listenFd = open_listenfd(numberPort);
    for (;;) {
        pthread_t t;
        struct sockaddr_storage clientAddr;

        socklen_t clientLen = sizeof(struct sockaddr);

        int connFd = accept(listenFd, (SA *) &clientAddr, &clientLen);
        if (connFd < 0) {
            fprintf(stderr, "Failed to accept\n");
            continue;
        }

        char hostBuf[MAXBUF], svcBuf[MAXBUF];
        if (getnameinfo((SA *) &clientAddr, clientLen,
                        hostBuf, MAXBUF, svcBuf, MAXBUF, 0) == 0)
            printf("Connection from %s:%s\n", hostBuf, svcBuf);
        else
            printf("Connection from ?UNKNOWN?\n");
        sss->connFd = connFd;
        serve_http(sss->connFd);

        close(connFd);
        printf("DEBUG: Success\n");

        pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
        pthread_cond_t con = PTHREAD_COND_INITIALIZER;
        pthread_create(&t, NULL, conn_handler, sss);
//        free(sss);
    }
}

int cnt = 0;
struct twoTime arrayLine[300];

void* conn_handler(void *) {
    serve_http(sss->connFd);
    close(sss->connFd);
    return NULL;
}

int main(int argc, char **argv) {

    while (1) {
        static struct option long_options[] =
                {
                        {"root", required_argument, 0, 0},
                        {"port", required_argument, 0, 0},
                        {NULL, 0, NULL, 0}
                };
        int option_index = 0;

        int ttt = getopt_long(argc, argv, "", long_options, &option_index);
        if (ttt == -1)
            break;
        switch(ttt){
            case 0:
                if (!strcmp(long_options[option_index].name, "root")){
                    memcpy(path, optarg, strlen(optarg));
                    memcpy(path2, optarg, strlen(optarg));
                }
                else if
                    (!strcmp(long_options[option_index].name, "port")){
                    memcpy(port, optarg, strlen(optarg));
                    memcpy(numberPort, optarg, strlen(optarg));
                }
                break;
        }
    }
    run();
    return 0;
}
//References
//https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html
//Ref to inclass 11 code from my group
