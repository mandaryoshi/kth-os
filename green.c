#include <stdlib.h>
#include <ucontext.h>
#include <stdio.h>
#include <assert.h>
#include "green.h"

#include <signal.h>
#include <sys/time.h>

#define PERIOD 100

#define FALSE 0
#define TRUE 1

#define STACK_SIZE 4096

static sigset_t block;

void timer_handler(int);

typedef struct queue {
  struct green_t *head;
  struct green_t *tail;
} queue;

queue *readyqueue;

int flag = 0;
green_cond_t condition;

green_mutex_t mutex;

static void enqueue(queue *queue, green_t *thread) {
  if (queue->head == NULL || queue->tail == NULL) {
    queue->head = thread;
    queue->tail = thread;
  } else {
    queue->tail->next = thread;
    queue->tail = thread;
  }
}

static green_t *dequeue(queue *queue) {
  green_t *thread;

  thread = queue->head;
  queue->head = queue->head->next;
  if (thread->next == NULL) {
    queue->tail == NULL;
  }
  thread->next = NULL;

  return thread;
}

static ucontext_t main_cntx = {0};
static green_t main_green= {&main_cntx, NULL, NULL, NULL, NULL, FALSE};

static green_t *running = &main_green;

static void init() __attribute__((constructor));

void init() {
  readyqueue = malloc(sizeof(queue));

  sigemptyset(&block);
  sigaddset(&block, SIGVTALRM);

  struct sigaction act = {0};
  struct timeval interval;
  struct itimerval period;

  act.sa_handler = timer_handler;
  assert(sigaction(SIGVTALRM, &act, NULL) == 0);

  interval.tv_sec = 0;
  interval.tv_usec = PERIOD;
  period.it_interval = interval;
  period.it_value = interval;
  setitimer(ITIMER_VIRTUAL, &period, NULL);

  getcontext(&main_cntx);
}

//-----BLOCK - sigprocmask(SIG_BLOCK, &block, NULL);
//-----UNBLOCK - sigprocmask(SIG_UNBLOCK, &block, NULL);

void blocksignal() {
  sigprocmask(SIG_BLOCK, &block, NULL);
}

void unblocksignal() {
  sigprocmask(SIG_UNBLOCK, &block, NULL);
}

void timer_handler(int sig) {
  green_t *susp = running;

  //add the running to the ready queue
  enqueue(readyqueue, susp);

  //find the next thread for execution
  green_t *next = dequeue(readyqueue);

  running = next;
  swapcontext(susp->context, next->context);
}

void green_thread() {
  green_t *this = running;

  (*this->fun)(this->arg);

  // place waiting (joining) thread in ready queue
  blocksignal();
  if (this->join) {
    enqueue(readyqueue, this->join);
  }
  unblocksignal();

  // free alocated memory structures
  free(this->context->uc_stack.ss_sp);
  free(this->context);

  // we're a zombie
  this->zombie = TRUE;

  // find the next thread to run
  blocksignal();
  green_t *next = dequeue(readyqueue);

  running = next;
  setcontext(next->context);
  unblocksignal();
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
  blocksignal();
  enqueue(readyqueue, new);
  unblocksignal();

  return 0;
}

int green_yield() {
  blocksignal();
  green_t * susp = running;
  // add susp to ready queue
  enqueue(readyqueue, susp);

  // select the next thread for execution
  green_t *next = dequeue(readyqueue);

  running = next;
  swapcontext(susp->context, next->context);
  unblocksignal();
  return 0;
}

int green_join(green_t *thread) {

  if(thread->zombie)
    return 0;

  blocksignal();

  green_t *susp = running;
  // add to waiting threads
  //??
  /*while (thread->join) {
    thread = thread->join;
  }*/

  thread->join = susp;

  //select the next thread for execution
  green_t *next = dequeue(readyqueue);

  running = next;
  swapcontext(susp->context, next->context);
  unblocksignal();
  return 0;
}

void green_cond_init(green_cond_t *cond) {
  cond->queue = malloc(sizeof(queue));

  /*cond->queue->head = NULL;
  cond->queue->tail = NULL;*/
}


void green_cond_wait(green_cond_t *cond){
  blocksignal();

  green_t *susp = running;
  //printf("hi\n");

  enqueue(cond->queue, susp);


  running = dequeue(readyqueue);
  swapcontext(susp->context, running->context);
  unblocksignal();

  /*self->next = cond->head;
  cond->head = running;*/

}


void green_cond_signal(green_cond_t *cond){
  /*while (cond->head != NULL) {
    cond->head = cond->head->next;
  }*/
  if (cond->queue->head == NULL) {
    return;
  }
  blocksignal();
  green_t *thread = dequeue(cond->queue);
  enqueue(readyqueue, thread);
  unblocksignal();
}

int green_mutex_init(green_mutex_t *mutex) {
  mutex->taken = FALSE;
  mutex->susp = NULL;
}

int green_mutex_lock(green_mutex_t *mutex) {
  // block timer interrupt
  blocksignal();

  green_t *susp = running;
  while(mutex->taken) {
    // suspend the running thread
    mutex->susp = susp;

    // find the next thread
    green_t *next = dequeue(readyqueue);

    running = next;
    swapcontext(susp->context, next->context);
  }
  // take the lock
  mutex->taken = TRUE;

  // unblock
  unblocksignal();

  return 0;
}

int green_mutex_unlock(green_mutex_t *mutex) {
  // block timer interrupt
  blocksignal();

  // move suspended threads to ready queue
  green_t *thread = mutex->susp;
  enqueue(readyqueue, thread);

  // release lock
  mutex->taken = FALSE;

  // unblock
  unblocksignal();

  return 0;
}

/*void *test(void *arg) {
  int i = *(int*)arg;
  int loop = 40000;
  while(loop > 0 ) {
    printf("thread %d: %d\n", i, loop);
    loop--;
    green_yield();
  }
}*/

/*void *test(void *arg) {
  int i = *(int*)arg;
  int loop = 40000;
  while(loop > 0 ) {
    if (flag == i) {
      printf("thread %d: %d\n", i, loop);
      loop--;
      flag = (i + 1) % 2;
      green_cond_signal(&condition);
    } else {
      green_cond_wait(&condition);
    }
  }
}*/

void *test(void *arg) {
  int i = *(int*)arg;
  int loop = 40000;
  green_mutex_lock(&mutex);
  while(loop > 0 ) {
    if (flag == i) {
      printf("thread %d: %d\n", i, loop);
      loop--;
      flag = (i + 1) % 2;
      green_cond_signal(&condition);
      green_mutex_unlock(&mutex);
    } else {
      green_cond_wait(&condition);
    }
  }
}



int main() {
  green_t g0, g1;
  int a0 = 0;
  int a1 = 1;
  green_mutex_init(&mutex);
  green_cond_init(&condition);
  green_create(&g0, test, &a0);
  green_create(&g1, test, &a1);

  green_join(&g0);
  green_join(&g1);
  printf("done\n");
  return 0;
}
