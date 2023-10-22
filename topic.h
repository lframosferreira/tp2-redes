#ifndef TOPIC_H
#define TOPIC_H

#include "common.h"

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
  char name[TOPIC_SIZE];
  int subscribed_clients[MAX_CLIENTS];
  struct Topic *next;
};

// COMO A LISTA É GLOBAL ELA NÃO PRECISA SER PASSADA COMO PARÂMETRO
struct Topic *get_or_create_topic(const char *topic_name);

// armazena em topcis_names os nomes dos topicos separados por ;
void get_topics_names(char *topics_names);

void remove_client_from_topics(const int client_id);

#endif