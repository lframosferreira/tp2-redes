#include "topic.h"

struct Topic *list_of_topics = NULL;

struct Topic *get_or_create_topic(const char *topic_name) {

  // Caso degenerado em que a lista inicial é vazia.
  if (list_of_topics == NULL) {
    struct Topic *new_topic = (struct Topic *)malloc(sizeof(struct Topic));
    if (new_topic == NULL) {
      err_n_die("Error while allocating memory using malloc()\n.");
    }
    strcpy(new_topic->name, topic_name);
    memset(new_topic->subscribed_clients, 0,
           sizeof(new_topic->subscribed_clients));
    new_topic->next = NULL;
    list_of_topics = new_topic;
    return new_topic;
  }

  struct Topic *curr = list_of_topics;
  while (strcmp(topic_name, curr->name) != 0) {
    if (curr->next == NULL) {
      break;
    }
    curr = curr->next;
  }

  // O loop anterior chegou até o último elemento da lista, e esse último
  // elemento não é igual ao nome do tópico
  if (curr->next == NULL && strcmp(topic_name, curr->name) != 0) {
    struct Topic *new_topic = (struct Topic *)malloc(sizeof(struct Topic));
    if (new_topic == NULL) {
      err_n_die("Error while allocating memory using malloc()\n.");
    }
    strcpy(new_topic->name, topic_name);
    memset(new_topic->subscribed_clients, 0,
           sizeof(new_topic->subscribed_clients));
    new_topic->next = NULL;
    curr->next = new_topic;
    return new_topic;
  }

  return curr;
}

void get_topics_names(char *topics_names) {
  if (list_of_topics == NULL) {
    strcpy(topics_names, "no topics available");
  } else {
    struct Topic *curr = list_of_topics;
    while (curr != NULL) {
      strcat(topics_names, curr->name);
      if (curr->next != NULL) {
        strcat(topics_names, ";");
      }
      curr = curr->next;
    }
  }
}

/* // Desalocando memória utilizada para a lista de tópicos
struct Topic *curr = list_of_topics;
while (curr != NULL) {
  struct Topic *aux = curr;
  curr = curr->next;
  free(aux);
} */