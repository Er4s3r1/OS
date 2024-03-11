#include <iostream>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <zmq.h>

#define SERVER_PORT "tcp://localhost:9145" // Сервер отправляет сообщения клиентам
#define CLIENT_PORT "tcp://localhost:9146" // Клиенты отправляет сообщения серверу

char my_name[100];
int can_write = 1;
int in_a_game = 0;
char name_my_game[100];

int main() {

    int id_message = 0;

    void *context = zmq_ctx_new(); // Создаём контекст (контейнер для всех сокетов в одном процессе) 
    void *serverSubscriber = zmq_socket(context, ZMQ_SUB); // Сокет для принятия сообщений
    
    zmq_connect(serverSubscriber, SERVER_PORT); // Подключаемся к адресу
    zmq_setsockopt(serverSubscriber, ZMQ_SUBSCRIBE, "", 0); // Подписываемся на все сообщения (пустая строка)
    
    void *publisher = zmq_socket(context, ZMQ_PUSH); // Сокет для отправки сообщений
    zmq_connect(publisher, CLIENT_PORT); // Привязываем сокет к адресу клиента

    char buffer[1024]; // Буфер для принятого сообщения
    char message[10000];
    char command[100];// Хранение команды
    char command_serv[100];
    char possible_name[100];
    char possible_name_game[100];
    char result[100];
    char answer[100];
    char input[100000];


    printf("Введите ваше имя:\n");
    if (fgets(input, sizeof(input), stdin) == NULL) { // Считываем вводную строку (NULL)
            printf("ERROR: Выключаюсь");
            exit(0);
    }
    sscanf(input, "%s", my_name);
    printf("\n");

    int check_name = 0;
    while (check_name == 0) {
        memset(buffer, 0, sizeof(buffer)); // Очистка buffer
        memset(message, 0, sizeof(message));
        memset(command, 0, sizeof(command));
        memset(possible_name, 0, sizeof(possible_name));
        memset(result, 0, sizeof(result));
        memset(input, 0, sizeof(input));


        sprintf(message, "InitName %s %d", my_name, id_message); 
        zmq_send(publisher, message, strlen(message), 0); // Отправили имя
        id_message++;
        // printf("DEBUG: Жду ответа от сервера\n");

        zmq_recv(serverSubscriber, buffer, sizeof(buffer), 0); // Ждём подтверждение
        // printf("DEBUG: Получил ответ от сервера\n");

        sscanf(buffer, "%s %s %s", command, possible_name, result); // Считываем начальное слово в command
        if ((strcmp(command, "AnswerName") == 0) && (strcmp(possible_name, my_name) == 0)) {

            if (strcmp(result, "okey") == 0) {
                printf("Хорошо, давайте начнем игру!\n");
                check_name = 1;
            }
            else if (strcmp(result, "repeat") == 0) {
                printf("Простите, это имя уже занято, попробуйте снова:\n");
                memset(my_name, 0, sizeof(my_name));
                memset(input, 0, sizeof(input));
                if (fgets(input, sizeof(input), stdin) == NULL) { // Считываем вводную строку (NULL)
                        printf("ERROR: Выключаюсь");
                        exit(0);
                }
                sscanf(input, "%s", my_name);
            }
        }
        else if (strcmp(command, "ServerWasKilled") == 0) {
            printf("ERROR: Сервер выключен\n");
            break;
        }
        else {
            printf("ERROR: Что-то не так...\n");
        }

    }
    printf("Вы можете ввести:\n newgame [имя игры] - создать новую игру\n connect [имя игры] - подключиться к игре\n leave - покинуть игру\n\n");

    while (1) {
        memset(buffer, 0, sizeof(buffer)); // Очищаем buffer
        memset(message, 0, sizeof(message));
        memset(command, 0, sizeof(command));
        memset(command_serv, 0, sizeof(command_serv));
        memset(possible_name, 0, sizeof(possible_name));
        memset(possible_name_game, 0, sizeof(possible_name_game));
        memset(result, 0, sizeof(result));
        memset(input, 0, sizeof(input));
        memset(answer, 0, sizeof(answer));


        if (in_a_game == 0) { // Еcли не в игре

            if (can_write) {
                if (fgets(input, sizeof(input), stdin) == NULL) { // Считываем вводную строку (NULL)
                    printf("ERROR: Выключаюсь");
                    exit(0);
                }

                sscanf(input, "%s", command); // Считываем команду
                printf("\n");

                if (strcmp(command, "newgame") == 0) {
                    sscanf(input, "%*s %s", result);

                    memset(message, 0, sizeof(message));
                    sprintf(message, "create %s %s %d", result, my_name, id_message); // создаем строку message
                    zmq_send(publisher, message, strlen(message), 0);
                    id_message++;
                    can_write = 0;

                }
                else if (strcmp(command, "connect") == 0) {
                    sscanf(input, "%*s %s", result);

                    memset(message, 0, sizeof(message));
                    sprintf(message, "connect %s %s %d", result, my_name, id_message); // создаем строку message
                    zmq_send(publisher, message, strlen(message), 0);
                    id_message++;
                    can_write = 0;
                    
                }
                else if (strcmp(command, "killserver") == 0) {
                    int keyyy;
                    sscanf(input, "%*s %d", &keyyy);
                    memset(message, 0, sizeof(message));
                    sprintf(message, "KillServer %d", keyyy); // Создаем строку message
                    zmq_send(publisher, message, strlen(message), 0);
                    id_message++;
                }
            }
            else {
                zmq_recv(serverSubscriber, buffer, sizeof(buffer), 0); // Получает сообщения от сокета и отправляет в буфер
                sscanf(buffer, "%s %s %s %s", command_serv, possible_name, possible_name_game, answer); // Читаем команду
                if (strcmp(command_serv, "Private") == 0 && strcmp(possible_name, my_name) == 0) {
                    if (strcmp(answer, "CreateSuccess") == 0) {
                        printf("Вы в игре!\n");
                        in_a_game = 1;
                        can_write = 1;
                        strcpy(name_my_game, possible_name_game);
                    }
                    else if (strcmp(answer, "CreateNotSuccess") == 0) { 
                        printf("Игра уже существует\n");
                        can_write = 1;
                    }
                    else if (strcmp(answer, "ConnectSuccess") == 0) {
                        printf("Вы в игре!\n");
                        in_a_game = 1;
                        can_write = 0;
                        printf("Подождите своей очереди\n");
                        strcpy(name_my_game, possible_name_game);
                    }
                    else if (strcmp(answer, "ConnectNotSuccess") == 0) {
                        printf("Этой игры не существует\n");
                        can_write = 1;
                    }
                }
                else if (strcmp(command, "ServerWasKilled") == 0) {
                    printf("ERROR: Сервер выключен\n");
                    break;
                }
            }

        }
        else {
            if (can_write) {
                printf("Пожалуйста, введите ваш ответ:\n");
                if (fgets(input, sizeof(input), stdin) == NULL) { // Считываем вводную строку (NULL)
                        printf("ERROR: Выключаюсь\n");
                        exit(0);
                    }
                

                sscanf(input, "%s", result);
                printf("\n");

                // Если клиент хочет покинуть игру
                if (strcmp(result, "leave") == 0) {
                    memset(message, 0, sizeof(message));
                    sprintf(message, "LeaveGame %s %s %d", name_my_game, my_name, id_message); // Создаем строку message
                    zmq_send(publisher, message, strlen(message), 0);
                    id_message++;
                    in_a_game = 0;
                    can_write = 1;
                    printf("Вы покинули игру\nПожалуйста, создайте новую игру или присоеденитесь к существующей\n");
                    continue;
                }

                memset(message, 0, sizeof(message));
                sprintf(message, "TryAnswer %s %s %s %d", name_my_game, my_name, result, id_message); // Создаем строку message
                zmq_send(publisher, message, strlen(message), 0);
                id_message++;

                can_write = 0;
            }
            else {
                memset(buffer, 0, sizeof(buffer));
                zmq_recv(serverSubscriber, buffer, sizeof(buffer), 0); 
                memset(command, 0, sizeof(command));
                sscanf(buffer, "%s", command);
                if (strcmp(command, "Checked") == 0) {
                    int cows;
                    int bulls;
                    sscanf(buffer, "%*s %s %s %s %d %d", possible_name_game, possible_name, result, &cows, &bulls);
                    if (strcmp(possible_name_game, name_my_game) == 0)
                        printf("Игрок '%s' попробовал: '%s'. Коровы: %d, Быки: %d\n", possible_name, result, cows, bulls);
                }
                else if (strcmp(command, "Win") == 0) {
                    int cows;
                    int bulls;
                    sscanf(buffer, "%*s %s %s %s %d %d",possible_name_game, possible_name, result, &cows, &bulls);
                    if (strcmp(possible_name_game, name_my_game) == 0) {
                        if (strcmp(possible_name, my_name) == 0)
                            printf("    -= Ты победил! =-\n\n");
                        else
                            printf("Игрок '%s' победил с попыткой '%s'\n", possible_name, result);
                        
                        in_a_game = 0;
                        can_write = 1;
                        printf("Пожалуйста, создайте новую игру или подключитесь к существующей:\n");
                    }
                }
                else if (strcmp(command, "YourTurn") == 0) {
                    sscanf(buffer, "%*s %s %s",possible_name_game, possible_name);
                    if (strcmp(possible_name_game, name_my_game) == 0 && strcmp(possible_name, my_name) == 0) {
                        can_write = 1;
                    }
                    else if ( strcmp(possible_name_game, name_my_game) == 0 ) {
                        printf("Ожидаем игрока: %s\n", possible_name);
                    }
                }
                else if (strcmp(command, "LeaveGAME") == 0) {
                    sscanf(buffer, "%*s %s %s",possible_name_game, possible_name);
                    if(strcmp(possible_name_game, name_my_game) == 0 ) {
                        printf("Игрок '%s' покинул игру\n", possible_name);
                    }
                }
                else if (strcmp(command, "ServerWasKilled") == 0) {
                    printf("ERROR: Сервер выключен\n");
                    break;
                }
            }
        }
    }

    zmq_close(publisher);
    zmq_close(serverSubscriber);
    zmq_ctx_destroy(context);

}
