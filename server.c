#include "topic.h"

pthread_mutex_t topics_list_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t clients_list_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

volatile int clients_list[MAX_CLIENTS] = {0};

int test_and_set_client_lowest_id() {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (clients_list[i] == 0) {
      clients_list[i] = 1;
      return i + 1;
    }
  }
  return -1;
}

void *handle_client(void *csockfd_ptr) {
  int csockfd = *((int *)csockfd_ptr);
  free(csockfd_ptr);
  struct BlogOperation operation;
  memset(&operation, 0, sizeof(operation));

  // Tratamento da conexão inicial, onde o servidor atribui um ID ao cliente que
  // acabou de se conectar e envia uma mensagem de volta com o ID escolhido.
  ssize_t bytes_received = recv(csockfd, &operation, sizeof(operation), 0);
  if (bytes_received == -1) {
    err_n_die("Erro no recv() de um client\n");
  } else if (bytes_received == 0) {
    fprintf(stdout, "client disconnected\n");
    exit(EXIT_SUCCESS);
  }

  pthread_mutex_lock(&clients_list_mutex);
  operation.client_id = test_and_set_client_lowest_id();
  if (operation.client_id == -1) {
    fprintf(
        stderr,
        "Todos os id's estão ocupados. Já existem 10 clientes no servidor.\n");
  }
  pthread_mutex_unlock(&clients_list_mutex);
  operation.operation_type = NEW_CONNECTION;
  operation.server_response = 1;
  strcpy(operation.topic, "");
  strcpy(operation.content, "");

  if (send(csockfd, &operation, sizeof(operation), 0) == -1) {
    err_n_die("Error on sending first message of connection to client.\n");
  }

  fprintf(stdout, "client %02d connected\n", operation.client_id);

  for (;;) {
    memset(&operation, 0, sizeof(operation));
    ssize_t bytes_received = recv(csockfd, &operation, sizeof(operation), 0);
    if (bytes_received == -1) {
      err_n_die("Error when using recv().\n");
    } else if (bytes_received == 0) {
      break;
    }

    struct Topic *topic;

    switch (operation.operation_type) {
    case NEW_POST_IN_TOPIC:
      pthread_mutex_lock(&topics_list_mutex);
      topic = get_or_create_topic(operation.topic);
      pthread_mutex_unlock(&topics_list_mutex);

      // redirect to other threads MAIN
      fprintf(stdout, "new post added in %s by %02d\n", operation.topic,
              operation.client_id);
      break;
    case LIST_TOPICS:
      // fzr checagem se tamanho dos nomes de topicos somados da menso
      // que 2048
      get_topics_names(operation.content);

      break;
    case SUBSCRIBE_IN_TOPIC:
      pthread_mutex_lock(&topics_list_mutex);
      topic = get_or_create_topic(operation.topic);
      if (topic->subscribed_clients[operation.client_id - 1] == 1) {
        strncpy(operation.content, "error: already subscribed\n", CONTENT_SIZE);
      } else {
        topic->subscribed_clients[operation.client_id - 1] = 1;
      }
      pthread_mutex_unlock(&topics_list_mutex);
      break;
    case UNSUBSCRIBE_FROM_TOPIC:
      // erro se nao tiver inscrito no topico?
      // topico sempre existe, se nao existir devo lançar erro?
      break;
    case DISCONNECT_FROM_SERVER:
      // Garantir que cliente é retirado da lista de inscrito dos tópicos aos
      // quais pertencia
      pthread_mutex_lock(&topics_list_mutex);
      remove_client_from_topics(operation.client_id);
      pthread_mutex_unlock(&topics_list_mutex);

      // Garantir que o id dele está liberado agora
      pthread_mutex_lock(&clients_list_mutex);
      clients_list[operation.client_id - 1] = 0;
      pthread_mutex_unlock(&clients_list_mutex);

      close(csockfd);
      fprintf(stdout, "client %02d was disconnected\n", operation.client_id);
      return NULL;
      break;
    default:
      fprintf(stderr, "error: command not found\n");
      break;
    }

    if (send(csockfd, &operation, sizeof(operation), 0) == -1) {
      err_n_die("Error using send().\n");
    }
  }
  return NULL;
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

    pthread_t t;
    int *csockfd_ptr = (int *)malloc(sizeof(int));
    *csockfd_ptr = csockfd;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_create(&t, &attr, handle_client, csockfd_ptr);

    pthread_attr_destroy(&attr);
  }

  close(sockfd);

  return EXIT_SUCCESS;
}