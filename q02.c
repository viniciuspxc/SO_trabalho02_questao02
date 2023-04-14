/* din_philo.c */
#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define n_filosofos 5
#define DELAY 50
#define FOOD 50
#define LOOP 1000

void *filosofo(void *id);
int checar_garfo(int, int, char *);
void pegar_garfo(int, int, char *);
void soltar_garfo(int, int);
int get_food();

pthread_mutex_t garfo[n_filosofos];
pthread_t filosofos[n_filosofos];
pthread_mutex_t food_lock;
int sleep_seconds = 0;

int main() {
  int i;
  for (int loop = 0; loop < LOOP; loop++) {
    pthread_mutex_init(&food_lock, NULL);
    for (i = 0; i < n_filosofos; i++)
      pthread_mutex_init(&garfo[i], NULL);
    for (i = 0; i < n_filosofos; i++)
      pthread_create(&filosofos[i], NULL, filosofo, (void *)i);
    for (i = 0; i < n_filosofos; i++)
      pthread_join(filosofos[i], NULL);
    printf("Fim do loop %d\n", loop);
  }
  return 0;
}

void *filosofo(void *num) {
  int id;
  int garfo_esquerda, garfo_direita;
  static int prato;
  int pratos_recebidos = 0;
  int prato_extra = 0;

  id = (int)num;
  printf("Filosofo[%d] termina de pensar.\n", id);
  garfo_esquerda = id;
  garfo_direita = id + 1;
  if (garfo_direita == n_filosofos)
    garfo_direita = 0;

  do {
    if (checar_garfo(id, garfo_direita, "direita") &&
        checar_garfo(id, garfo_esquerda, "esquerda")) {

      pegar_garfo(id, garfo_direita, "direita");
      pegar_garfo(id, garfo_esquerda, "esquerda");

      prato = get_food();
      pratos_recebidos++;
      printf("Filosofo[%d] comendo prato %d.\n", id, prato);
      usleep(DELAY * (FOOD - prato + 1));
      soltar_garfo(garfo_esquerda, garfo_direita);
    }
    if (id < (FOOD % n_filosofos) && prato_extra < 1) {
      prato_extra++;
    }

  } while (pratos_recebidos < (FOOD / n_filosofos) + prato_extra);

  printf("\n-- Filosofo[%d] termina de comer com %d pratos --\n\n", id,
         pratos_recebidos);
  return (NULL);
}
int get_food() {
  static int prato = FOOD;
  int meu_prato;
  pthread_mutex_lock(&food_lock);
  if (prato > 0) {
    prato--;
  }
  meu_prato = prato;
  pthread_mutex_unlock(&food_lock);
  return meu_prato;
}

int checar_garfo(int filosofo, int garfo_n, char *hand) {
  if (pthread_mutex_trylock(&garfo[garfo_n]) == 0) {
    pthread_mutex_unlock(&garfo[garfo_n]);
    return 1;
  } else {
    return 0;
  }
}

void pegar_garfo(int filosofo, int garfo_n, char *hand) {
  pthread_mutex_lock(&garfo[garfo_n]);
  printf("Filosofo[%d] pega garfo %d na %s.\n", filosofo, garfo_n, hand);
}

void soltar_garfo(int c1, int c2) {
  pthread_mutex_unlock(&garfo[c1]);
  pthread_mutex_unlock(&garfo[c2]);
}