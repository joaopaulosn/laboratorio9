#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>


#define TAM_BUFFER 100

sem_t empty, full;

int buffer[TAM_BUFFER];


void inserir(int element) {
  static int in = 0;
  sem_wait(&empty);
  buffer[in] = element;
  in = (in + 1) % TAM_BUFFER;
  sem_post(&full);
}

int remove() {
  int item;
  static int out = 0;
  sem_wait(&full);
  item = buffer[out];
  out = (out + 1) % TAM_BUFFER;
  sem_post(&empty);
  return item;
}

void *produtor(void *params) {
  int elemento;
  int contador = 0;
  while(1) {
    elemento = contador++;
    inserir(elemento);
    printf("Inserido --> %d\n", elemento);
  }
  pthread_exit(NULL);
}

void *consumidor(void *params) {
  int elemento;
  while(1) {
    elemento = remove();
    printf("Removido --> %d\n", elemento);
  }
  pthread_exit(NULL);
}

int main(int argc, char const *argv[]) {
  srand(time(NULL));
  pthread_t threads[2];
  int *threadsIds;
  threadsIds = malloc(sizeof(int) * 2);

  sem_init(&empty, 0, TAM+TAM_BUFFER);
  sem_init(&full, 0, 0);

  for(int i = 0; i < 4; i++) {
    threadsIds[i] = i;
  }

  pthread_create(&threads[0], NULL, produtor, (void *) &threadsIds[0]);
  pthread_create(&threads[1], NULL, consumidor, (void *) &threadsIds[1]);

  for (int i = 0; i < 4; i++) {
    pthread_join(threads[i], NULL);
  }

  return 0;
}