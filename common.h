#ifndef COMMON_H
#define COMMON_H

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// Blog operation
#define TOPIC_SIZE 50
#define CONTENT_SIZE 2048

// checar se deve ser enum

#define NEW_CONNECTION 1
#define NEW_POST_IN_TOPIC 2
#define LIST_TOPICS 3
#define SUBSCRIBE_IN_TOPIC 4
#define DISCONNECT_FROM_SERVER 5

#define MAX_CLIENTS 10
#define MAX_BUFFER_SIZE 1024

struct BlogOperation {
  int client_id;
  int operation_type;
  int server_response;
  char topic[TOPIC_SIZE];
  char content[CONTENT_SIZE];
};

void err_n_die(const char *msg);
void server_usage(FILE *fp, const char *path);
void client_usage(FILE *fp, const char *path);

int addrparse(const char *addrstr, const char *portstr,
              struct sockaddr_storage *storage);

int server_sockaddr_init(const char *addr_family, const char *porstr,
                         struct sockaddr_storage *storage);

/* Conforme especificado no enunciado, só existirão no máximo 10 clientes
 * conectados por vez no servidor, e ao se desconectar um cliente é retirado da
 * lista de clientes associados referentes aos tópicos qem que ele estava
 * inscrito. Desse modo, para manter controle de quais clientes estão associados
 * a um tópico, basta utilizar um vetor booleano.
 *
 *
 * A quantidade de tópicos, no entanto, possui um tamanho máximo não
 * determinado. Dessa forma, ela será implementada como uma lista encadeada. */

struct Topic {
  char *name[TOPIC_SIZE];
  int subscribed_clients[MAX_CLIENTS];
  struct Topic *next;
};

struct Topic *get_or_create_topic(struct Topic *head, const char *topic_name);

// armazena em topcis_names os nomes dos topicos separados por ;
void get_topics_names(char *topics_names, struct Topic *head);


#endif