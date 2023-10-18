#ifndef COMMON_H
#define COMMON_H

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define TOPIC_SIZE 50
#define CONTENT_SIZE 2048


// checar se deve ser enum

#define NEW_CONNECTION 1
#define NEW_POS_IN_TOPIC 2
#define LIST_TOPICS 3
#define SUBSCRIBE_IN_TOPIC 4
#define DISCONNECT_FROM_SERVER 5

struct BlogOperation {
    int client_id;
    int operation_type;
    int server_response;
    char topic[TOPIC_SIZE];
    char content[CONTENT_SIZE];
};






#endif