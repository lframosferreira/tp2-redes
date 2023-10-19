#include "common.h"

volatile int client_id_count = 0;

struct Topic *list_of_topics = NULL;

void handle_client(int csockfd) {

  struct BlogOperation
      operation; // checar se é ok fzr isso e nao alocar na heap
  memset(&operation, 0, sizeof(operation));

  // primeria conexao
  ssize_t bytes_received = recv(csockfd, &operation, sizeof(operation), 0);
  if (bytes_received == -1) {
    err_n_die("Erro no recv() de um client\n");
  } else if (bytes_received == 0) {
    fprintf(stdout, "client disconnected\n");
    exit(EXIT_SUCCESS);
  }

  operation.client_id = ++client_id_count;
  operation.operation_type = NEW_CONNECTION;
  operation.server_response = 1;
  strcpy(operation.topic, "");
  strcpy(operation.content, "");

  if (send(csockfd, &operation, sizeof(operation), 0) == -1) {
    err_n_die("Error on sending first message of connection to client.\n");
  }

  fprintf(stdout, "client %01d connected\n", operation.client_id);

  const int CLIENT_ID = operation.client_id;

  for (;;) {
    memset(&operation, 0, sizeof(operation));
    ssize_t bytes_received = recv(csockfd, &operation, sizeof(operation), 0);
    if (bytes_received == -1) {
      err_n_die("Error when using recv().\n");
    } else if (bytes_received == 0) {
      break;
    }

    switch (operation.operation_type) {
    case NEW_POST_IN_TOPIC:
      break;
    case LIST_TOPICS:
      if (list_of_topics == NULL) {
        strncpy(operation.content, "no topics available", CONTENT_SIZE);
      } else { // fzr checagem se tamanho dos nomes de topicos somados da menso
               // que 2048
        get_topics_names(operation.content, list_of_topics);
      }
      break;
    case SUBSCRIBE_IN_TOPIC:
      struct Topic *topic =
          get_or_create_topic(list_of_topics, operation.topic);
      if (topic->subscribed_clients[operation.client_id - 1] == 1) {
        strncpy(operation.content, "error: already subscribed\n", CONTENT_SIZE);
      } else {
        topic->subscribed_clients[operation.client_id - 1] = 1;
      }
      break;
    case DISCONNECT_FROM_SERVER:
      break;
    default:
      fprintf(stderr, "nao entendi\n");
      break;
    }

    if (send(csockfd, &operation, sizeof(operation), 0) == -1) {
      err_n_die("Error using send().\n");
    }
  }
}

int main(int argc, char **argv) {

  if (argc != 3) {
    server_usage(stderr, argv[0]);
    exit(EXIT_FAILURE);
  }

  char *addr_family = argv[1];
  char *portstr = argv[2];

  struct sockaddr_storage servaddr;
  memset(&servaddr, 0, sizeof(servaddr));

  if (server_sockaddr_init(addr_family, portstr, &servaddr) == -1) {
    err_n_die("Error while using server_sockaddr_init().\n");
  }

  int sockfd = socket(servaddr.ss_family, SOCK_STREAM, 0);
  if (sockfd == -1) {
    err_n_die("Error while opening server socket.\n");
  }

  if (bind(sockfd, (struct sockaddr *)(&servaddr), sizeof(servaddr)) == -1) {
    close(sockfd);
    err_n_die("Error in bind().\n");
  }

  if (listen(sockfd, MAX_CLIENTS) == -1) {
    close(sockfd);
    err_n_die("Error on listen().\n");
  }

  struct sockaddr_storage client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  for (;;) {

    int csockfd =
        accept(sockfd, (struct sockaddr *)(&client_addr), &client_addr_len);
    if (csockfd == -1) {
      err_n_die("Error on using accept().\n");
    }

    // threads here

    handle_client(csockfd);
  }

  close(sockfd);

  return EXIT_SUCCESS;
}