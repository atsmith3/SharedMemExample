/*
 * Andrew Smith
 * Shared Memory Producer/Consumer Example
 * 020820
 */

#include <fcntl.h> 
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/shm.h> 
#include <sys/stat.h>
#include <unistd.h>
#include <semaphore.h>

#define SHM_LENGTH   sizeof(mapped)
#define STR_LENGTH   4096
#define SHM_NAME     "shm_test"

typedef struct {
  sem_t sem;
  uint8_t new_data;
  uint8_t done;
  char str[STR_LENGTH];
} mapped;

int main(int argc, char** argv) {
#ifdef PRODUCER
  // Init Shared Memory:
  int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
  if(shm_fd == -1) {
    fprintf(stderr,"Failed to create & open /dev/shm/%s\n", SHM_NAME);
    exit(-1);
  }
  ftruncate(shm_fd, SHM_LENGTH);
  mapped* shm_ptr = (mapped*)mmap(NULL, SHM_LENGTH, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);
  if(shm_ptr == NULL) {
    fprintf(stderr,"mmap failed\n");
    shm_unlink(SHM_NAME);
    exit(-1);
  }

  // Init shared mem struct:
  sem_init(&shm_ptr->sem, 1, 1);
  shm_ptr->new_data = 0;
  shm_ptr->done = 0;
  shm_ptr->str[0] = 0;

  uint8_t was_consumed = 0;
  for(int i = 0; i < 20; i++) {
    was_consumed = 0;
    while(was_consumed == 0) { 
      sem_wait(&shm_ptr->sem);
      if(shm_ptr->new_data == 0) {
        was_consumed = 1;
        snprintf(shm_ptr->str, STR_LENGTH, "%s %d\n", "Hello", i);
        shm_ptr->new_data = 1;
        if(i == 20-1) {
          shm_ptr->done = 1;
        }
      }
      sem_post(&shm_ptr->sem);
    }
  }

  munmap(shm_ptr, SHM_LENGTH);
  shm_unlink(SHM_NAME);
  return 0;
#else
  int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
  if(shm_fd == -1) {
    fprintf(stderr,"Failed to open /dev/shm/%s\n", SHM_NAME);
    exit(-1);
  }
  mapped* shm_ptr = (mapped*)mmap(NULL, SHM_LENGTH, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);
  if(shm_ptr == NULL) {
    fprintf(stderr,"mmap failed\n");
    shm_unlink(SHM_NAME);
    exit(-1);
  }

  int done = 0;
  while(done == 0) {
    sem_wait(&shm_ptr->sem);
    if(shm_ptr->new_data == 1) {
      printf("%s", shm_ptr->str);
      shm_ptr->new_data = 0;
    }
    done = shm_ptr->done;
    sem_post(&shm_ptr->sem);
  }

  munmap(shm_ptr, SHM_LENGTH);
  shm_unlink(SHM_NAME);
  return 0;
#endif
}
