#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tree.h"
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <zmq.h>

int id;

// typedef struct pair {
//     int first;
//     int second;
// } pair;

int main(int argc, char **argv) {
    sscanf(argv[1], "%d", &id);
    // printf("Ok: %d:\n", id);
    // printf("Hello world!\n");
    void *context = zmq_ctx_new();
    void *subscriber = zmq_socket(context, ZMQ_SUB);
    zmq_connect(subscriber, "tcp://127.0.0.1:5555");
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "", 0);

    char buffer[256];
    char command[256];
    char myvar[10] = "MyVar";
    int values[1000];
    int argss[1000];
    char id_str[sizeof(int)];
    int arg1 = 0, arg2 = 0, k = 0, c = 0, val = 0, cur_arg1 = 0, cur_val = 0, i = 0;
    bool flag, check, already_given, first_time;
    pid_t id1, id2;
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        memset(command, 0, sizeof(command));
        // memset(&id, 0, sizeof(int)); c

        zmq_recv(subscriber, buffer, sizeof(buffer), 0);
        // printf("message: %s\n", buffer);
        sscanf(buffer, "%s", command);

        if (strcmp(command, "create") == 0) {
            sscanf(buffer, "%*s %d %d", &arg1, &arg2);
            if (id == arg2) {
                if (arg1 > id) k += 1;
                else c += 1;
                if ((arg1 < 5 && arg2 > 5) || (arg1 > 5 && arg2 < 5)) continue;
                if (c > 1) {
                    printf("Nope_c\n");
                    continue;
                }
                else if (k > 1) {
                    printf("Nope_c\n");
                    continue;
                    
                }
                printf("Node: %d create child\n", id);
                id1 = fork();
                if (id1 < 0) {
                    perror("fork");
                }
                if (id1 == 0){
                    char str1[sizeof(int)];
                    sprintf(str1, "%d", arg1);
                    execl("./user", "./user", str1, NULL); 
                    perror("execl");
                }
                else {
                    printf("Ok, let's go: %d\n", id1);
                    printf("\n");
                }
            }
        }
        else if (strcmp(command, "exec") == 0) {
            sscanf(buffer, "%*s %d %*s %d", &arg1, &val);
            if (id == arg1) {
                if (cur_val != val) {
                    printf("Val has changed\n");
                    flag = false;
                }
                cur_val = val;
                if (flag == true) {
                    printf("OK:%d %d\n", argss[i], values[i]);
                }
                if (val != NULL && flag != true) {
                    printf("OK:%d\n", arg1);
                    values[i] = val;
                    argss[i] = arg1;
                    flag = true;
                }
                if (flag != true && val == NULL) {
                    printf("OK:%d MyVar not found\n", arg1);
                }
            }
        }
        else if (strcmp(command, "kill") == 0) {
            sscanf(buffer, "%*s %d", &arg1);
            if (id == arg1){
                printf("Node %d: has been killed\n", id);
                break;
            }  
        }

    }
    zmq_close(subscriber);
    zmq_ctx_destroy(context);
}
