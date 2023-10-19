#include "common.h"

volatile int client_id_count = 0;





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

  for (;;) {
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