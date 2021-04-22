# COMP SCI 537 DIS-313 Week 12
Contents:
- P7 Background
  - Pthread
  - Shared Memory
  - Signal Handler
- P7 Spec Overview

References:
- Linux manual [page](https://man7.org/linux/man-pages/man7/pthreads.7.html) for pthreads.

## P7 Spec Overview
In this project, you will basically do two things:
1. Extend a single-threaded web server to a multi-threaded web server. You don't need to worry about the detail about HTTP or anyother thing about processing the HTTP request. The main focus is
  - Implement a fixed size shared buffer that follows a producer-consumer protocol.
  - Implement a pool of threads that will grab the work from the shared queue.
We will talk about pthread in the background part.
2. The second part is about how to share data across the processes. You need to use shared memory to achieve that in this project.

## P7 Background
### Pthread
You need to add `-lpthread` flag to indicate that you are linking against thre pthread library. A very simple pthread code would look as following
~~~[c]
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

#define POOL_SIZE 4
void *worker(void *arg) {
  printf("Worker thread %d starts\n", (uintptr_t)arg);
  return 0;
}

int main(void) {
  pthread_t thread_pool[POOL_SIZE];

  for (int i = 0; i < POOL_SIZE; i ++)
    pthread_create(&thread_pool[i], NULL, worker, (void *)i);

  for (int i = 0; i < POOL_SIZE; i++)
    pthread_join(thread_pool[i], NULL);
  
  printf("Main Thread exits\n");
}
~~~

If you want more synchronization between thread, then you might need to use mutex and conditional variable. A simple program using the mutex and conditional variable would look as following.
~~~[c]
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#define POOL_SIZE 4
pthread_mutex_t mu;
pthread_cond_t c;

int turn = 0;

void *worker (void *arg) {
    uintptr_t index = (uintptr_t) arg;
    pthread_mutex_lock (&mu);
    while (turn != index)
        pthread_cond_wait (&c, &mu);
    printf ("Worker thread %ld's turn\n", (uintptr_t) arg);
    turn++;
    pthread_cond_broadcast (&c);
    pthread_mutex_unlock (&mu);
    return 0;
}

int main (void) {
    pthread_t thread_pool[POOL_SIZE];

    for (int i = 0; i < POOL_SIZE; i++)
        pthread_create (&thread_pool[i], NULL, worker, (void *) i);

    for (int i = 0; i < POOL_SIZE; i++)
        pthread_join (thread_pool[i], NULL);

    printf ("Main Thread exits\n");
}
~~~

### Shared Memory
You need `-lrt` compiler flag to tell the compiler linked against the real-time library. The following is a set of function calls you might find useful.
~~~[c]
  // Open a shared memory file named shm-kaiwei
  // O_RDWR means read and write permission
  // O_CREAT means create if not exist
  // S_IRUSR | S_IWUSR means the permission of the file if creeated
  int shmfd = shm_open("shm-kaiwei", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

  // Adjust the size of the shared memory file
  ftruncate(shmfd, 4096);

  // Memory the shared memory file into the memory
  // PROT_WRITE means we can write to it
  // MAP_SHARED means the change would be propogated to the file
  char *shm_ptr = mmap(NULL, pagesize, PROT_WRITE, MAP_SHARED, shmfd, 0);

  // Write the shared memory region
  strcpy(shm_ptr, "COMP SCI 537");

  // Unmap the memory region
  munmap(shm_ptr, pagesize);

  // Delete the shared memory region.
  shm_unlink("shm-kaiwei");
~~~

### Signal Handler
In this project, we need to deal with signal handler. The idea of the handler is just that when some event is triggered, the corresponding handler will be called.
~~~[c]
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

void sigint_handler(int sig) {
    printf("Captured signal SIGINT %d\n", sig);
    exit(0);
}

int main(void) {
    // Register the signal handler
    signal(SIGINT,  sigint_handler);
    while (1) {}
    return 0;
}
~~~