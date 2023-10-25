#ifndef TOPIC_H
#define TOPIC_H

#include "common.h"

/* Estrutura auxiliar para armazenar informações acerca um tópico.
 * Todo tópico possui um nome que o caracteriza.
 * Conforme especificado no enunciado, só existirão no máximo 10 clientes
 * conectados por vez no servidor, e, ao se desconectar, um cliente é retirado
 * da lista de clientes associados referentes aos tópicos qem que ele estava
 * inscrito. Desse modo, para manter controle de quais clientes estão associados
 * a um tópico, basta utilizar um vetor booleano de tamanho 10.
 *
 *
 * A quantidade de tópicos, no entanto, possui um tamanho máximo não
 * determinado. Dessa forma, ela será implementada como uma lista encadeada. */

struct Topic {
  char name[TOPIC_SIZE];
  int subscribed_clients[MAX_CLIENTS];
  struct Topic *next;
};

/**
 * @brief Busca pelo nome de um tópico na lisat encadeada global de tópicos. Se
 * encontrar, retorna um ponteiro para o tópico. Se não encontrar, cria ele e
 * armazena na lista.
 *
 * @param topic_name Nome do tópico que deve ser encontrado ou criado
 * @return struct Topic * : ponteiro para o tópico encontrado/criado
 */
struct Topic *get_or_create_topic(const char *topic_name);

/**
 * @brief Armazena em topics_names os nomes dos tópicos presentes na lista
 * encadeada global separados por ;
 *
 * @param topics_names Buffer onde os nomes concatenados dos tópicos devem ser
 * armazenados
 * @return void
 */
void get_topics_names(char *topics_names);

/**
 * @brief Remove o client com identificador client_id de todos os tópicos nos
 * quais ele estava inscrito.
 *
 * @param client_id Identificador do cliente
 * @return void
 */
void remove_client_from_topics(const int client_id);

#endif