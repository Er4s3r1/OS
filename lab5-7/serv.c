#include "tree.h"
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <mqueue.h>
#include <zmq.h>

int accessed = 0;
int locker = 0;
char message[256];

void killwithchildren(node* tree, void * publisher) {
    if (tree == NULL) {
        return;
    }
    sprintf(message, "kill %d", tree->key);
    zmq_send(publisher, message, strlen(message), 0);
    memset(message, 0, sizeof(message));

    killwithchildren(tree->left, publisher);
    killwithchildren(tree->right, publisher);
}

int main() {
    void *context = zmq_ctx_new();
    void *publisher = zmq_socket(context, ZMQ_PUB);
    zmq_bind(publisher, "tcp://127.0.0.1:5555");
    
    char input[256];
    char command[100];
    char id_str[sizeof(int)];
    int arg1 = 0, arg2 = 0, k = 0, c = 0, kk = 0, cc = 0, val = 0, cur_arg2 = 0;
    pid_t id1, id2;
    bool check1 = true, flag = false, is_parent = false, is_not_parent = false;
    node *tree = create(tree, 5); //пусть управляющий узел будет иметь айдишник 5

    while (1) {
        memset(message, 0, sizeof(message));
        memset(command, 0, sizeof(command));
        memset(id_str, 0, sizeof(id_str));

        if(fgets(input, sizeof(input), stdin) == NULL) { //если заканчивается ввод, то прога кончается (ctrl + D)
            break;
        }

        sscanf(input, "%s", command);

        if (strcmp(command, "create") == 0) { // команда create 
            sscanf(input, "%*s %d %d", &arg1, &arg2);
            // printf("Cur_arg2: %d, arg2: %d\n", cur_arg2, arg2);
            node *child = search(tree, arg1);
            node* parent = search(tree, arg2);
            if (parent == NULL) {
                printf("Error: No such parent\n");
                continue;
            }
            else if (child != NULL) {
                printf("Error: Already exist\n");
                continue;
            }
            else {
                if (arg2 != cur_arg2) {
                    kk = 0;
                    cc = 0;
                }
                if (arg2 == 5) {
                        if (arg1 > arg2) k += 1;
                        else c += 1;
                        if (k > 1) {
                            printf("Nope\n");
                            continue;
                        }
                        if (c > 1) {
                            printf("Nope\n");
                            continue;
                        }
                        tree = add(tree, arg1);
                        print_tree(tree, 0);
                        printf("\n");
                        id1 = fork();
                        if (id1 == -1) {
                            perror("fork");
                            return -1;
                        }
                        else if (id1 == 0) {
                            sprintf(id_str, "%d", arg1);
                            execl("./user", "./user", id_str, NULL);
                            perror("execl");
                        }
                        else {
                            printf("Ok: %d\n", id1);
                            is_parent = true;
                            printf("\n");
                        }
                }
                else {
                    cur_arg2 = arg2;
                    if (arg1 > arg2) kk += 1;
                    else cc += 1;
                    if (kk > 1) {
                        printf("Nope\n");
                        continue;
                    }
                    if (cc > 1) {
                        printf("Nope\n");
                        continue;
                    }                 
                    sprintf(message, "create %d %d", arg1, arg2);
                    zmq_send(publisher, message, strlen(message), 0);
                    tree = add(tree, arg1);
                    print_tree(tree, 0);
                    printf("\n");
                }
            }
        }

        else if (strcmp(command, "exec") == 0) { //команда exec
            sscanf(input, "%*s %d", &arg1);
            node *searching = search(tree, arg1);
            if (arg1 == 5) {
                printf("This is a command process\n");
            }
            else if (searching == NULL) {
                printf("Error:id Not found\n");
            }
            else {
                printf("message sent to id %d\n", arg1);
                zmq_send(publisher, input, strlen(input), 0);

            }
        }

        else if (strcmp(command, "kill") == 0) {
            sscanf(input, "%*s %d", &arg1);
            node *search_to_kill = search(tree, arg1);
            if (search_to_kill == NULL) {
                printf("Error: id Not Found\n");
                continue;
            }
            if (arg1 == 5) {
                printf("You can't kill control procces\n");
                continue;
            }
            else {
                if (arg1 > 5) {
                    killwithchildren(search_to_kill->right, publisher);
                    zmq_send(publisher, input, strlen(input), 0);

                    disableNode(search_to_kill->right);
                    disableOneNode(search_to_kill);
                }
                else {
                    killwithchildren(search_to_kill->left, publisher);
                    zmq_send(publisher, input, strlen(input), 0);
                    printf("message sent\n");

                    disableNode(search_to_kill->left);
                    disableOneNode(search_to_kill);
                }
            }
            
        }
        else if (strcmp(command, "ping") == 0) {
            sscanf(input, "%*s %d", &arg1);
            node* search_available_root = search(tree, arg1);
            if (arg1 == 5) {
                printf("Control process is always available\n");
                printf("\n");
            }
            if (search_available_root == NULL) {
                printf("Error: id not found\n");
                continue;
            }
            if (search_available_root->exist) {
                printf("Ok: 1 // узел %d доступен\n", arg1);
                continue;
            }
            else {
                printf("Ok: 0 // узел %d недоступен\n", arg1);
                continue;
            }
        }
        
    }
    zmq_close(publisher);
    zmq_ctx_destroy(context);
}
