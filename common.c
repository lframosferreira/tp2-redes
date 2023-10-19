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

int addrparse(const char *addrstr, const char *portstr,
              struct sockaddr_storage *storage) {
  if (addrstr == NULL || portstr == NULL) {
    return -1;
  }
  uint16_t port = (uint16_t)atoi(portstr);
  if (port == 0) {
    return -1;
  }
  port = htons(port);

  memset(storage, 0, sizeof(*storage));

  struct in_addr inaddr4;
  if (inet_pton(AF_INET, addrstr, &inaddr4) == 1) {
    struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
    addr4->sin_family = AF_INET;
    addr4->sin_port = port;
    addr4->sin_addr = inaddr4;
    return 0;
  }

  struct in6_addr inaddr6;
  if (inet_pton(AF_INET6, addrstr, &inaddr6) == 1) {
    struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
    addr6->sin6_family = AF_INET6;
    addr6->sin6_port = port;
    memcpy(&(addr6->sin6_addr), &inaddr6, sizeof(inaddr6));
    return 0;
  }

  return -1;
}

int server_sockaddr_init(const char *addr_family, const char *portstr,
                         struct sockaddr_storage *storage) {
  if (addr_family == NULL || portstr == NULL) {
    return -1;
  }
  uint16_t port = (uint16_t)atoi(portstr);
  if (port == 0) {
    return -1;
  }
  port = htons(port);

  memset(storage, 0, sizeof(*storage));

  if (strcmp(addr_family, "v4") == 0) {
    struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
    addr4->sin_family = AF_INET;
    addr4->sin_port = port;
    addr4->sin_addr.s_addr = INADDR_ANY;
    return 0;
  }

  if (strcmp(addr_family, "v6") == 0) {
    struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
    addr6->sin6_family = AF_INET6;
    addr6->sin6_port = port;
    addr6->sin6_addr = in6addr_any;
    return 0;
  }

  return -1;
}