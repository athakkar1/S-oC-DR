#include <fcntl.h>
#include <netinet/in.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "shared_memory.h"

char *shared_block;

int main(int argc, char **argv) {
  uint16_t port = 2000;
  struct sockaddr_in servaddr; /* server address */
  shared_block = attach_memory_block(FILENAME, BLOCK_SIZE);
  sem_unlink(SEM_CONSUMER_NAME);
  sem_unlink(SEM_PRODUCER_NAME);

  sem_t *sem_prod = sem_open(SEM_PRODUCER_NAME, O_CREAT | O_EXCL, 0660, 0);
  if (sem_prod == SEM_FAILED) {
    perror("sem_open failed for prod");
    return 0;
  }

  sem_t *sem_cons = sem_open(SEM_CONSUMER_NAME, O_CREAT | O_EXCL, 0660, 1);
  if (sem_prod == SEM_FAILED) {
    fprintf(stderr, "semaphore creation failed\n");
    return 0;
  }

  if (shared_block == NULL) {
    fprintf(stderr, "shared_block creation failed\n");
  }

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    fprintf(stderr, "socket creation failed\n");
    return 0;
  }
  memset((char *)&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);
  if (inet_pton(AF_INET, "192.168.1.154", &servaddr.sin_addr) <= 0) {
    fprintf(stderr, "inet_pton failed\n");
    return 0;
  }
  if (connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    perror("connect error");
    exit(EXIT_FAILURE);
  }
  while (true) {
    sem_wait(sem_prod);
    if (send(sock, shared_block, BLOCK_SIZE, 0) < 0) {
      fprintf(stderr, "send failed\n");
      return 0;
    }
    sem_post(sem_cons);
  }

  close(sock);
  detach_memory_block(FILENAME);
  sem_close(sem_prod);
  sem_close(sem_cons);
}