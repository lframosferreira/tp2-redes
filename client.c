#include "common.h"


int main(int argc, char **argv){

  if (argc != 3) {
    client_usage(stderr, argv[0]);
    exit(EXIT_FAILURE);
  }



  return EXIT_SUCCESS;
}