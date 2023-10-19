#include "common.h"

int main(int argc, char **argv) {

  if (argc != 3) {
    client_usage(stderr, argv[0]);
    exit(EXIT_FAILURE);
  }
  char *addrstr = argv[1];
  char *portstr = argv[2];

  struct sockaddr_storage servaddr;
  if (addrparse(addrstr, portstr, &servaddr) == -1) {
    err_n_die("Error while using addrparser().\n");
  }

  int sockfd = socket(servaddr.ss_family, SOCK_STREAM, 0);
  if (sockfd == -1) {
    err_n_die("Erro while opening client socket.\n");
  }

  if (connect(sockfd, (struct sockaddr *)(&servaddr), sizeof(servaddr)) == -1) {
    close(sockfd);
    err_n_die("Error while connecting to server.\n");
  }

  char input_buffer[MAX_BUFFER_SIZE];

  // lógica da mensagem inicial enviada quando a conexao é feita
  struct BlogOperation operation;
  operation.client_id = 0;
  operation.operation_type = NEW_CONNECTION;
  operation.server_response = 0;
  strcpy(operation.topic, "");
  strcpy(operation.content, "");

  if (send(sockfd, &operation, sizeof(operation), 0) == -1) {
    close(sockfd);
    err_n_die(
        "Error on using send() for the first message after connection.\n");
  }

  ssize_t bytes_received = recv(sockfd, &operation, sizeof(operation), 0);
  if (bytes_received == -1) {
    err_n_die("Error when using recv() for the first connection with server.\n");
  } else if (bytes_received == 0) {
    err_n_die("Server didn't respond.\n");
  }


  for (;;) {
    memset(input_buffer, 0, sizeof(input_buffer));
    if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
      close(sockfd);
      err_n_die("Error while reading client input using fgets().\n");
    }

    input_buffer[strcspn(input_buffer, "\n")] = '\0';

    char *command;
    char *topic;

    command = strtok(input_buffer, " ");

    if (strcmp(command, "subscribe") == 0) {
      topic = strtok(NULL, " ");
    } else if (strcmp(command, "unsubscribe") == 0) {
      topic = strtok(NULL, " ");
    } else if (strcmp(command, "publish")) {
      strtok(NULL, " "); // discard in
      topic = strtok(NULL, " ");
    } else if (strcmp(command, "list")) { // list topics case (maybe put it
                                          // before all others?)

    } else if (strcmp(command, "exit")) {

    } else {
      fprintf(stderr, "error: command not found\n");
    }

    if (send(sockfd, input_buffer, sizeof(input_buffer), 0) == -1) {
      close(sockfd);
      err_n_die("Error on using send().\n");
    }
  }

  close(sockfd);
  return EXIT_SUCCESS;
}