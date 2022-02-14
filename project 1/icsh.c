#include <stdio.h>
//#include <intrin.h>
#include <string.h>
#include <dirent.h>
//#include <process.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/fcntl.h>


#define	MAX_SIZE_CMD	256
#define	MAX_SIZE_ARG	16


typedef struct string string;
char *arr[256];
char cmd[MAX_SIZE_CMD];
char *argv[MAX_SIZE_ARG];			// an array for command and arguments
char last[MAX_SIZE_CMD];
char *cpy[MAX_SIZE_ARG];
char *file[MAX_SIZE_ARG];
char test[MAX_SIZE_ARG];
int childPid;
int inIndex(char** args);
int outIndex(char** args);


void get_cmd();								// get command string from the user
void c_shell();
void stringArr();
void printArr();
void exitt();
char readFile();
void sig_suspend();
void sig_kill();
int IORedirection();


int main(int argc, char* argv[]){

    if (argc >= 2){
        cpy[0] = argv[1];
        readFile();
    }else if (argc <= 1){
        // start the shell
        c_shell();
    }
}

void c_shell() {
    while (1) {
        get_cmd();
        signal(SIGTSTP, sig_suspend);
        signal(SIGINT, sig_kill);

        if(strcmp("!!", cmd)){
            memset(last,NULL, 0);           //reset value of last so that it doesn't mess up if run for multiple time
            memcpy(last, cmd, strlen(cmd)+1);      // set last = cmd
            memset(arr,NULL,64);
        }


        stringArr(cmd);
        outIndex(arr);
        inIndex(arr);
        // bypass empty commands
        if (!strcmp("", cmd)) {
            continue;
        }
            // check for "exit" command
        else if (!strcmp("exit", arr[0])){
            if(arr[1] == NULL){
                printf("bad command\n");
                continue;
            }
            else{
                if(arr[1] <= 255 && arr[1] >= 1){
                    printf("bye\n");
                    exit(arr[1]);
                }else{
                    exitt();
                    printf("bye\n");

                }
            }
            break;
        }

        else if (!strcmp("!!", arr[0])) {
            printf("%s\n", last);
            stringArr(last);
            printArr(arr);
        }
        else if(!strcmp("echo", arr[0])){
            printArr(arr);
        }
        else{
            childPid = fork();
            if(childPid == 0) {
                childPid = getpid();
                IORedirection(arr);
                if (execvp(arr[0], arr) == -1) {
                    printf("bad command\n");
                }

            }
            wait(NULL);
            continue;
        }
    }
}

char readFile(){
//    char test[] = "C:\\Users\\uSeR\\Desktop\\testcode\\";
    strcat(test, cpy[0]);
    FILE *fp = fopen ( test, "r");
    char line[1024];
    char ch = getc ( fp );
    int index = 0;
    while ( ch != EOF ) {
        if ( ch != '\n'){
            line[index++] = ch;
        }else {
            line[index] = '\0';
            index = 0;
//            printf ( "%s\n", line );

            stringArr(line);
            if (!strcmp("exit", arr[0])){
                exitt();
                printf("Bye\n");
            }
            else if (!strcmp("!!", arr[0])) {
                stringArr(last);
                printArr(arr);
            }
            else if(!strcmp("echo", arr[0])){
                printArr(arr);
            }
            else{
                printf("bad command\n");
            }
        }
        ch = getc ( fp );
    }
    fclose ( fp );

    return 0;
}


void printArr(char *arr2[]){

    int length = sizeof(arr2)/sizeof(char);
    for(int i = 1; i < length; i++){
        if(arr2[i] != NULL) {
            printf("%s ", arr2[i]);
        } else{
            printf("\n");
            break;
        }
    }
}

//From Stack overflow and modified
void stringArr(char *c){
    int f = 0;
    char *b = strtok(c, " ");
    while (b != NULL){
        arr[f++] = b;
        b = strtok(NULL, " ");
    }
}

void get_cmd(){
    // get command from user
    printf("icsh $>\t");
    fgets(cmd, MAX_SIZE_CMD, stdin);
    // remove trailing newline
    if ((strlen(cmd) > 0) && (cmd[strlen (cmd) - 1] == '\n'))
        cmd[strlen (cmd) - 1] = '\0';
}

void exitt(){
    arr[1] = atoi(arr[1]) % 266;
    printf("bye\n");
    exit(arr[1]);

}

void sig_suspend(){
    kill(childPid, SIGTSTP);
    printf("Process Suspended\n");
    c_shell();
}

void sig_kill(){
    kill(childPid, SIGINT);
    printf("Process Killed\n");
//    c_shell();
}

int outIndex(char** arr){
    for (int tt = 0; arr[tt] != NULL; tt++) {
        if (strcmp(arr[tt], ">") == 0) {
            return tt;
        }
    }
    return -1;
}

int inIndex(char** arr){
    for (int bb = 0; arr[bb] != NULL; bb++) {
        if (strcmp(arr[bb], "<") == 0) {
            return bb;
        }
    }
    return -1;
}


int IORedirection(char** list){

    char* in;
    char* out;
    FILE* rfp;
    FILE* wfp;
    int input, output;

    input = inIndex(list);
    output = outIndex(list);


    if (input >= 0) {

        in = list[input + 1];
        list[input] = NULL;
        rfp = freopen(in, "r", stdin);

    }

    if (output >= 0) {

        out = list[output + 1];
        list[output] = NULL;
        wfp = freopen(out, "w", stdout);

    }
}




