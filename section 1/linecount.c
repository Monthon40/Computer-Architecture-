#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "string.h"

#define BUFSIZE 120

int main(int argc, char *argv[]) {

    if (!strstr(argv[1], ".txt")) {
        printf("Binary file. Bye.\n");
        exit(0);
    }
    //Open file
    int cnt = 0;
    char buf[BUFSIZE];
    int fd = open(argv[1], O_RDONLY);
    ssize_t readNum;
    while ((readNum = read(fd, buf, BUFSIZE)) > 0) {
        for (int i = 0; i < readNum; i++) {
            if (buf[i] == '\n') {
                cnt++;
            }
        }
    }
    printf("Number of lines: %d\n", cnt);
    //close file
    close(fd);
    return 0;
}

//Ans 1 There is an optimal size to a buffer. Too small a buffer can trigger more system calls than necessary, while too big a buffer can trigger unnecessary reloads of the CPU cache.
//Ans 2 8K to 16K or 32K is a good balance between memory usage and performance. The size of the buffer will act differently depend on the data.