#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include "pcsa_net.h"
#include <stdio.h>
#include <sys/file.h>
#include "string.h"
#include <stdlib.h>
#include <sys/wait.h>
#include<unistd.h>
#include<signal.h>
#include <ctype.h>


#define BUFF 255

int ttt;

typedef struct sockaddr SA;

void recieve_logic(int connFd) {
    ssize_t readByte;
    char buf[BUFF];

    while ((readByte = read(connFd, buf, BUFF)) > 0) {
        printf("DEBUG: Read %ld bytes\n", readByte);

        ssize_t numToWrite = readByte;
        char *writeBuf = buf;

        while (numToWrite > 0) {
            ssize_t written = write(ttt, writeBuf, numToWrite);
            if (written < 0) {
                printf("ERROR: Couldn't write\n");
                break;
            }
            numToWrite -= written;
            writeBuf += written;
        }
    }
    printf("DEBUG: Connection was closed by the client");
}

int main(int argc, char *argv[]) {
    int sv = open_listenfd(argv[1]);
    //Creating output file
    int filefd = open(argv[2], O_WRONLY | O_CREAT, 0666);
    ttt = filefd;


    for (;;) {
        struct sockaddr_storage clientAddr;
        socklen_t clientLen = sizeof(struct sockaddr_storage);

        int connFd = accept(sv, (SA *) &clientAddr, &clientLen);

        char hostBuf[BUFF], svcBuf[BUFF];
        if (getnameinfo((SA *) &clientAddr, clientLen, hostBuf, BUFF, svcBuf, BUFF, 0) == 0) {
            printf("Connection from %s:%s\n", hostBuf, svcBuf);
        } else {
            printf("Connection from UNKNOWN.");
        }

        recieve_logic(connFd);

        close(connFd);
        close(filefd);
    }

    return 0;
}
