#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include "pcsa_net.h"
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

#define BUFF 1024


int main(int argc, char *argv[]) {
    ssize_t readByte;
    char buf[BUFF];
    int clientFd = open_clientfd(argv[1], argv[2]);
    int ttt = open(argv[3], O_RDONLY);
    if (ttt == -1) {
        fprintf(stderr, "ERROR reading, meh\n");
        exit(-1);
    } else {
        while (readByte = (read(ttt, buf, BUFF)) > 0) {
            ssize_t numToWrite = readByte;
            char *writeBuf = buf;
            while (numToWrite > 0) {
                write(clientFd, buf, BUFF);
            }
            numToWrite -= written;
            writeBuf += written;
        }
    }

    close(clientFd);
    close(ttt);
    return 0;
}
