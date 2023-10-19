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

  char input_buffer[1024];

  for (;;) {
    memset(input_buffer, 0, sizeof(input_buffer));
    if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
      close(sockfd);
      err_n_die("Error while reading client input using fgets().\n");
    }

    if (send(sockfd, input_buffer, sizeof(input_buffer), 0) == -1) {
      close(sockfd);
      err_n_die("Error on using send().\n");
    }
  }

  return EXIT_SUCCESS;
}