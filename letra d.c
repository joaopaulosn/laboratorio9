#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>


#define TAM_BUFFER 1000
#define NUM_THREADS 20

sem_t empty, full;
sem_t mutexProd;

int buffer[TAM_BUFFER];


void insert(int element) {
  static int in = 0;
  sem_wait(&empty);
  sem_wait(&mutexProd);
  buffer[in] = element;
  in = (in + 1) % TAM_BUFFER;
  sem_post(&mutexProd);
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
    insert(elemento);
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
  pthread_t threads[NUM_THREADS];
  int *threadsIds;
  threadsIds = malloc(sizeof(int) * NUM_THREADS);

  sem_init(&mutexProd, 0, 1);
  sem_init(&empty, 0, TAM_BUFFER);
  sem_init(&full, 0, 0);

  for(int i = 0; i < NUM_THREADS; i++) {
    threadsIds[i] = i;
  }

  pthread_create(&threads[0], NULL, consumidor, (void *) &threadsIds[0]);
  for(int i = 1; i < NUM_THREADS; i++) {
    pthread_create(&threads[i], NULL, produtor, (void *) &threadsIds[i]);
  }

  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  return 0;
}