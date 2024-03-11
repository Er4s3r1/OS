#include <iostream>
#include <fcntl.h>
#include <sys/wait.h>
#include <mqueue.h>
#include <zmq.h>
#include "lib.hpp"

#define SERVER_PORT "tcp://localhost:9145" // Сервер отправляет сообщения клиентам
#define CLIENT_PORT "tcp://localhost:9146" // Клиенты отправляет сообщения серверу
#define GAME_PORT "tcp://localhost:9147" // Игра отправляет сообщения серверу

#define MAX_COUNT_OF_GAMES 100
#define MAX_MAN_IN_GAME 100
#define MAX_NAME_OF_PLAYER 30
#define MAX_NAME_OF_GAME 40

char message[1000000];

int main() {
    printf("DEBUG SERVER: Сервер запущен\n");

    struct Dictionary my_dict = createDictionary();

    char *stringsClients[100000];
    int count_client = 0;

    void *context = zmq_ctx_new(); // Контекст

    // Создаем сокет для отправки сообщений
    void *publisher = zmq_socket(context, ZMQ_PUB);
    zmq_bind(publisher, SERVER_PORT);

    // Создаем сокет для получения сообщений от клиента
    void *clientSubscriber = zmq_socket(context, ZMQ_PULL);
    zmq_bind(clientSubscriber, CLIENT_PORT);

    // Создаем сокет для получения сообщений от игры
    void *gameSubscriber = zmq_socket(context, ZMQ_PULL); // Решил сделать через pull и push, потому что не мог делать много публикаторов и одного подписчика
    zmq_bind(gameSubscriber, GAME_PORT);
    
    // Стуктура для сообщений от client и game
    zmq_pollitem_t items_for_sockets[] = {
        { clientSubscriber, 0, ZMQ_POLLIN, 0 },
        { gameSubscriber, 0, ZMQ_POLLIN, 0 }
    };
    // Нужно для того, чтобы в цикле не ждать прихода сообщений, а проверять без блокировки


    char buffer_client[100000];
    char buffer_game[100000];
    char command[50]; // Хранение команды
    char lastMessage[100000];
    char nextValue[1000];

    char name_of_game[50];
    char name_of_client[50];
    while (1) { // Работает пока не будет введен Enter и не ждет, пока что-то введут!
        int rc = zmq_poll(items_for_sockets, 2, 0); // Неблокирующий вызов, возвращает, на скольких сокетах произошли изменения

        if (items_for_sockets[0].revents & ZMQ_POLLIN) { // Проверяем были ли изменения в этом сокете (ZMQ_POLLIN - хотя бы один байт данных может быть прочитан из файлового дескриптора без блокировки)
            printf("DEBUG SERVER: Получаю сообщение от клиента\n");
            memset(buffer_client, 0, sizeof(buffer_client)); // Очищаем buffer_client

            zmq_recv(clientSubscriber, buffer_client, sizeof(buffer_client), 0); // Принятие сообщения 
            

            memset(command, 0, sizeof(command));
            sscanf(buffer_client, "%s", command);
    
            if (strcmp(command, "create") == 0) {

                memset(name_of_game, 0, sizeof(name_of_game));
                memset(name_of_client, 0, sizeof(name_of_client));
                sscanf(buffer_client, "%*s %s %s", name_of_game, name_of_client);

                int found = keyExists(&my_dict, name_of_game);

                if(found) {
                    memset(message, 0, sizeof(message));
                    sprintf(message, "Private %s %s CreateNotSuccess", name_of_client, name_of_game); // Создаем строку message
                    zmq_send(publisher, message, strlen(message), 0);
                }
                else{
                    pid_t id = fork();
                    if (id == 0) {
                        execl("./game", "./game", name_of_game, NULL); 
                        perror("execl");
                    }
                    addToDictionary(&my_dict, name_of_game, name_of_client);

                    memset(message, 0, sizeof(message));
                    sprintf(message, "Private %s %s CreateSuccess", name_of_client, name_of_game); // Создаем строку message
                    zmq_send(publisher, message, strlen(message), 0);

                }

            }

            else if (strcmp(command, "connect") == 0) {

                memset(name_of_game, 0, sizeof(name_of_game));
                memset(name_of_client, 0, sizeof(name_of_client));
                sscanf(buffer_client, "%*s %s %s", name_of_game, name_of_client);

                int found = keyExists(&my_dict, name_of_game);
                printf("DEBUG SERVER: Попытка подключения клиента '%s', игра: %s; найдена: %d;\n", name_of_client, name_of_game, found);

                if (found) {
                    memset(message, 0, sizeof(message));
                    sprintf(message, "Private %s %s ConnectSuccess", name_of_client, name_of_game); // Создаем строку message
                    zmq_send(publisher, message, strlen(message), 0);
                    addToDictionary(&my_dict, name_of_game, name_of_client);
                }
                else {
                    memset(message, 0, sizeof(message));
                    sprintf(message, "Private %s %s ConnectNotSuccess", name_of_client, name_of_game); // Создаем строку message
                    zmq_send(publisher, message, strlen(message), 0);
                }
            }

            else if (strcmp(command, "TryAnswer") == 0) { // Проверяем предположеие игрока
                zmq_send(publisher, buffer_client, strlen(buffer_client), 0); // Отправляем его игре
                printf("DEBUG SERVER: Пытаюсь дать ответ клиенту '%s';\n", buffer_client);
            }

            else if (strcmp(command, "InitName") == 0) { // Проверяем, существует ли такое имя у игрока
                memset(name_of_client, 0, sizeof(name_of_client));
                sscanf(buffer_client, "%*s %s", name_of_client);

                int found = 0; // Флаг для указания наличия строки
                for (int i = 0; i < count_client + 1; ++i) {
                    if (stringsClients[i] != NULL && strcmp(stringsClients[i], name_of_client) == 0) {
                        found = 1;
                        break; // Нашли совпадение, выходим из цикла
                    }
                }
                if (found) {
                    memset(message, 0, sizeof(message));
                    sprintf(message, "AnswerName %s repeat", name_of_client); // Создаем строку message
                    zmq_send(publisher, message, strlen(message), 0);
                }
                else{
                    memset(message, 0, sizeof(message));
                    sprintf(message, "AnswerName %s okey", name_of_client); // Создаем строку message
                    zmq_send(publisher, message, strlen(message), 0);
                    stringsClients[count_client] = strdup(name_of_client);
                    count_client ++;
                }
            }
            else if(strcmp(command, "KillServer") == 0) {
                int keyyy;
                sscanf(buffer_client, "%*s %d", &keyyy);
                if (keyyy == 123456) {
                    memset(message, 0, sizeof(message));
                    sprintf(message, "ServerWasKilled"); // Создаем строку message
                    zmq_send(publisher, message, strlen(message), 0);
                    break;
                }

            }
            else if(strcmp(command, "LeaveGame") == 0) {
                memset(name_of_game, 0, sizeof(name_of_game));
                memset(name_of_client, 0, sizeof(name_of_client));
                sscanf(buffer_client, "%*s %s %s", name_of_game, name_of_client);

                // Дальше отправляем клиенту его ход
                memset(nextValue, 0, sizeof(nextValue));

                strcpy(nextValue,  getNextValue(&my_dict, name_of_game, name_of_client));
                if (nextValue != NULL) {
                    memset(message, 0, sizeof(message));
                    sprintf(message, "LeaveGAME %s %s", name_of_game, name_of_client); // Создаем строку message
                    zmq_send(publisher, message, strlen(message), 0);

                    printf("DEBUG SERVER: Следующее значение: %s;\n", nextValue);

                    memset(message, 0, sizeof(message));
                    sprintf(message, "YourTurn %s %s", name_of_game, nextValue); // Создаем строку message
                    zmq_send(publisher, message, strlen(message), 0);
                }

                removePersonFromGameDictionary(&my_dict, name_of_game, name_of_client);
            }

            strcpy(lastMessage, buffer_client);
            printf("DEBUG SERVER: Последнее сообщение '%s'; Клиент буфер: '%s';\n", lastMessage, buffer_client);

        }

        if (items_for_sockets[1].revents & ZMQ_POLLIN) {
            printf("DEBUG SERVER: Получаю сообщение от game;\n");
            memset(buffer_game, 0, sizeof(buffer_game)); // Очищаем buffer_game

            zmq_recv(gameSubscriber, buffer_game, sizeof(buffer_game), 0);

            if (strcmp(buffer_game, lastMessage) == 0)
                continue; 

            memset(command, 0, sizeof(command));
            sscanf(buffer_game, "%s", command);

            if (strcmp(command, "Checked") == 0) {
                zmq_send(publisher, buffer_game, strlen(buffer_game), 0);
                sscanf(buffer_game, "%*s %s %s", name_of_game, name_of_client);

                // Дальше отправляем клиенту его ход
                memset(nextValue, 0, sizeof(nextValue));

                strcpy(nextValue,  getNextValue(&my_dict, name_of_game, name_of_client));
                if (nextValue != NULL) {
                    memset(message, 0, sizeof(message));
                    sprintf(message, "YourTurn %s %s", name_of_game, nextValue); // Создаем строку message
                    zmq_send(publisher, message, strlen(message), 0);
                }
            }
            else if(strcmp(command, "Win") == 0) {
                zmq_send(publisher, buffer_game, strlen(buffer_game), 0);
                sscanf(buffer_game, "%*s %s", name_of_game);
                removeFromDictionary(&my_dict, name_of_game);
            }

            strcpy(lastMessage, buffer_game);

        }

    }


    zmq_close(publisher);
    zmq_close(clientSubscriber);
    zmq_close(gameSubscriber);
    zmq_ctx_destroy(context);

}
