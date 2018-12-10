#include <stdlib.h>
#include <ucontext.h>
#include <stdio.h>
#include <assert.h>
#include "green.h"

#define FALSE 0
#define TRUE 1

#define STACK_SIZE 4096

typedef struct queue {
  struct green_t *head;
  struct green_t *tail;
} queue;

queue readyqueue = {NULL, NULL};

int flag = 0;
green_cond_t cond;

void enqueue(queue *queue, green_t *thread) {
  if (queue->head == NULL && queue->tail == NULL) {
    queue->head = thread;
    queue->tail = thread;
  } else {
    queue->tail->next = thread;
    queue->tail = thread;
  }
}

green_t *dequeue(queue *queue) {
  green_t *thread;

  thread = queue->head;
  queue->head = queue->head->next;
  /*if (queue.head->next == NULL) {
    readyQueue.tail == NULL;
  }*/
  thread->next = NULL;

  return thread;
}

static ucontext_t main_cntx = {0};
static green_t main_green= {&main_cntx, NULL, NULL, NULL, NULL, FALSE};

static green_t *running = &main_green;

static void init() __attribute__((constructor));

void init() {
  getcontext(&main_cntx);
}

void green_thread() {
  green_t *this = running;

  (*this->fun)(this->arg);

  // place waiting (joining) thread in ready queue
  if (this->join) {
    enqueue(&readyqueue, this->join);
  }

  // free alocated memory structures
  free(this->context->uc_stack.ss_sp);
  free(this->context);

  // we're a zombie
  this->zombie = TRUE;

  // find the next thread to run
  green_t *next = dequeue(&readyqueue);

  running = next;
  setcontext(next->context);
}

int green_create(green_t *new, void *(*fun)(void*), void *arg) {

  ucontext_t *cntx = (ucontext_t *)malloc(sizeof(ucontext_t));
  getcontext(cntx);

  void *stack = malloc(STACK_SIZE);

  cntx->uc_stack.ss_sp = stack;
  cntx->uc_stack.ss_size = STACK_SIZE;

  makecontext(cntx, green_thread, 0);
  new->context = cntx;
  new->fun = fun;
  new->arg = arg;
  new->next = NULL;
  new->join = NULL;
  new->zombie = FALSE;

  // add new to the ready queue
  enqueue(&readyqueue, new);

  return 0;
}

int green_yield() {
  green_t * susp = running;
  // add susp to ready queue
  enqueue(&readyqueue, susp);

  // select the next thread for execution
  green_t *next = dequeue(&readyqueue);

  running = next;
  swapcontext(susp->context, next->context);
  return 0;
}

int green_join(green_t *thread) {

  if(thread->zombie)
    return 0;

  green_t *susp = running;
  // add to waiting threads
  //??
  /*while (thread->join) {
    thread = thread->join;
  }*/

  thread->join = susp;

  //select the next thread for execution
  green_t *next = dequeue(&readyqueue);

  running = next;
  swapcontext(susp->context, next->context);
  return 0;
}

void green_cond_init(green_cond_t *cond) {
  cond->head = NULL;
  cond->tail = NULL;
}


void green_cond_wait(green_cond_t *cond){
  green_t *self = running;


  self->next = cond->head;
  cond->head = running;

}


void green_cond_signal(green_cond_t *cond){
  while (cond->head != NULL) {
    cond->head = cond->head->next;
  }

}

void *test(void *arg) {
  int i = *(int*)arg;
  int loop = 4;
  while(loop > 0 ) {
    printf("thread %d: %d\n", i, loop);
    loop--;
    green_yield();
  }
}

int main() {
  green_t g0, g1;
  int a0 = 0;
  int a1 = 1;
  green_create(&g0, test, &a0);
  green_create(&g1, test, &a1);

  green_join(&g0);
  green_join(&g1);
  printf("done\n");
  return 0;
}
