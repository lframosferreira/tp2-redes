#include "common.h"

void err_n_die(const char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

void server_usage(FILE *fp, const char *path) {
  char *basename = strrchr(path, '/');
  basename = basename ? basename + 1 : path;
  fprintf(fp, "usage: %s [IP_PROTOCOL] [PORT]\n", basename);
}

void client_usage(FILE *fp, const char *path) {
  const char *basename = strrchr(path, '/');
  basename = basename ? basename + 1 : path;
  fprintf(fp, "usage: %s [SERVER_ADDR] [PORT]\n", basename);
}