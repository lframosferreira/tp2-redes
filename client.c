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

  /* Lógica da mensagem inicial onde a conexção é estabelecida e o servidor
   * atribui um identificador único ao cliente. */
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
    err_n_die(
        "Error when using recv() for the first connection with server.\n");
  } else if (bytes_received == 0) {
    err_n_die("Server didn't respond.\n");
  }

  const int MY_ID = operation.client_id;

  for (;;) {
    memset(&operation, 0, sizeof(operation));
    operation.client_id = MY_ID;
    operation.server_response = 0;

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
      operation.operation_type = SUBSCRIBE_IN_TOPIC;
      strncpy(operation.topic, topic, TOPIC_SIZE);
      strcpy(operation.content, "");
    } else if (strcmp(command, "unsubscribe") == 0) {
      topic = strtok(NULL, " ");
      operation.operation_type = UNSUBSCRIBE_FROM_TOPIC;
      strncpy(operation.topic, topic, TOPIC_SIZE);
      strcpy(operation.content, "");
    } else if (strcmp(command, "publish") == 0) {
      /* Essa chamada de strtok() armazena em topic o
      valor 'in'. Ele deve ser ignorado. */
      topic = strtok(NULL, " ");
      topic = strtok(NULL, " ");
      if (fgets(operation.content, sizeof(operation.content), stdin) == NULL) {
        err_n_die("Error while reading client input using fgets().\n");
      }
      operation.content[strcspn(operation.content, "\n")] = '\0';
      operation.operation_type = NEW_POST_IN_TOPIC;
      strncpy(operation.topic, topic, TOPIC_SIZE);
    } else if (strcmp(command, "list") == 0) {
      operation.operation_type = LIST_TOPICS;
      strcpy(operation.topic, "");
      strcpy(operation.content, "");

    } else if (strcmp(command, "exit") == 0) {
      operation.operation_type = DISCONNECT_FROM_SERVER;
      strcpy(operation.topic, "");
      strcpy(operation.content, "");

    } else {
      fprintf(stderr, "error: command not found\n");
    }

    if (send(sockfd, &operation, sizeof(operation), 0) == -1) {
      close(sockfd);
      err_n_die("Error on using send().\n");
    }

    /* Se for mensagem de 'exit', o cliente se desconecta e não irá prosseguir
     * com o loop de troca de mensagens.*/
    if (operation.operation_type == DISCONNECT_FROM_SERVER) {
      break;
    }

    ssize_t bytes_received = recv(sockfd, &operation, sizeof(operation), 0);
    if (bytes_received == -1) {
      err_n_die("Error when using recv().\n");
    } else if (bytes_received == 0) {
      break;
    }

    switch (operation.operation_type) {

    case NEW_POST_IN_TOPIC:
      break;
    case LIST_TOPICS:
      fprintf(stdout, "%s\n", operation.content);
      break;
    case SUBSCRIBE_IN_TOPIC:
      break;
    case DISCONNECT_FROM_SERVER:
      break;
    default:
      break;
    }
  }
  close(sockfd);
  return EXIT_SUCCESS;
}