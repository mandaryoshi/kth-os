#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>


/* The list will contain elements from 0 to MAX-1 */

#define MAX 100

typedef struct cell {
  int val;
  struct cell *next;
} cell;

cell sentinel = {MAX, NULL};
cell dummy = {-1, &sentinel};

cell *global = &dummy;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


void toggle(cell *lst, int r) {
  cell *prev = NULL;
  cell *this = lst;
  pthread_mutex_lock(&mutex);

  while(this->val < r) {
      prev = this;
      this = this->next;
  }
  if(this->val == r) {
    /* found it - delete it */
    prev->next = this->next;
    free(this);
  } else {
    cell *new = malloc(sizeof(cell));
    // printf("a(%d)\n", r);
    new->val = r;
    new->next = this;
    prev->next = new;
  }
  pthread_mutex_unlock(&mutex);
  return;
}

typedef struct {int inc; int id; cell *list;} args;

void *bench(void *arg) {
  int inc = ((args*)arg)->inc;
  int id = ((args*)arg)->id;
  cell *lstp = ((args*)arg)->list;

  // printf("thread %d starts\n", id);

  for(int i = 0; i < inc; i++) {
    int r = rand() % MAX;
    toggle(lstp, r);
  }
}

int main(int argc, char *argv[]) {

  if(argc != 3) {
    printf("usage: list <total> <threads>\n");
    exit(0);
  }

  int n = atoi(argv[2]);
  int inc = (atoi(argv[1]) / n);

  pthread_t *thrt = malloc(n * sizeof(pthread_t));

  printf("%d threads doing %d operations each\n", n, inc);

  args *thra = malloc(n * sizeof(args));

  for(int i =0; i < n; i++) {
    thra[i].inc = inc;
    thra[i].id = i;
    thra[i].list = global;
  }

  struct timespec t_start, t_stop;

  clock_gettime(CLOCK_MONOTONIC_COARSE, &t_start);

  for(int i =0; i < n; i++) {
    pthread_create(&thrt[i], NULL, bench, &thra[i]);
  }

  for(int i =0; i < n; i++) {
    pthread_join(thrt[i], NULL);
  }


  clock_gettime(CLOCK_MONOTONIC_COARSE, &t_stop);

  long wall_sec = t_stop.tv_sec - t_start.tv_sec;
  long wall_nsec = t_stop.tv_nsec - t_start.tv_nsec;
  long wall_msec = (wall_sec *1000) + (wall_nsec / 1000000);

  printf("done in %ld ms\n", wall_msec);


  return 0;
}
