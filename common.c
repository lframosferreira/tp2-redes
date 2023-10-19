#include "common.h"

void err_n_die(const char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}