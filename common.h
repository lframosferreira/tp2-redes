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
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// Blog operation
#define TOPIC_SIZE 50
#define CONTENT_SIZE 2048

// Checar se isso deve ser enum
// ATENÇÃO: cuidado com valor de unsubscribe from topic. Olhar spec assim que
// monitora responder. Isso aqui é uma solução paliativa

#define NEW_CONNECTION 1
#define NEW_POST_IN_TOPIC 2
#define LIST_TOPICS 3
#define SUBSCRIBE_IN_TOPIC 4
#define DISCONNECT_FROM_SERVER 5
#define UNSUBSCRIBE_FROM_TOPIC 6

#define MAX_CLIENTS 10
#define MAX_BUFFER_SIZE 1024

struct BlogOperation {
  int client_id;
  int operation_type;
  int server_response;
  char topic[TOPIC_SIZE];
  char content[CONTENT_SIZE];
};

/**
 * @brief Imprime um erro e sai da execução com status de falha
 *
 * @param msg Mensagem de erro a ser impressa
 * @return void
 */
void err_n_die(const char *msg);

/**
 * @brief Imprime na tela o modo de uso do servidor
 *
 * @param fp Ponteiro para o arquivo onde o modo de uso deve ser escrito
 * @param path Caminho para o arquivo binário que contêm o executável do
 * servidor
 * @return void
 */
void server_usage(FILE *fp, const char *path);

/**
 * @brief Imprime na tela o modo de uso do cliente
 *
 * @param fp Ponteiro para o arquivo onde o modo de uso deve ser escrito
 * @param path Caminho para o arquivo binário que contêm o executável do cliente
 * @return void
 */
void client_usage(FILE *fp, const char *path);

// As funções abaixo foram baseadas nas implementações disponibilizadas na
// playlist do professor Ítalo Cunha
// https://www.youtube.com/watch?v=tJ3qNtv0HVs&t=2s

/**
 * @brief Realiza o 'parse' do endereço do servidor passado como parâmetro para
 * o cliente
 *
 * @param addrstr String que representa o endereço do servidor
 * @param portstr String que representa a porta onde o servidor está rodando
 * @param storage Struct em que o endereço será armazenado
 * @return int: -1 em caso de erro, 0 caso contrário
 */
int addrparse(const char *addrstr, const char *portstr,
              struct sockaddr_storage *storage);

/**
 * @brief Inicializa o servidor
 *
 * @param addr_family String que representa a familia do endereço, isto é, 'v4'
 * ou 'v6'
 * @param portstr String que representa a porta onde o servidor deve rodar
 * @param storage Struct em que o endereço será armazenado
 * @return int: -1 em caso de erro, 0 caso contrário
 */
int server_sockaddr_init(const char *addr_family, const char *porstr,
                         struct sockaddr_storage *storage);

void dbg(const char *msg);

#endif