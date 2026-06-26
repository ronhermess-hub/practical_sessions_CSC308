// Shared Memory Programming
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#define SHM_SIZE 1024
#define SEM_NAME "/shm_sem"
int main() {
    // 1. Create shared memory segment
    int shmid = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid < 0) { perror("shmget"); exit(1); }
    // 2. Create named semaphore for synchronization
    sem_unlink(SEM_NAME);
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0644, 0);
    if (sem == SEM_FAILED) { perror("sem_open"); exit(1); }
    pid_t pid = fork();
    if (pid == 0) {
        // --- CHILD PROCESS: reads from shared memory --
        char *shm_ptr = (char *)shmat(shmid, NULL, 0);
        if (shm_ptr == (char *)-1) { perror("shmat child"); exit(1); }
        sem_wait(sem); // wait until parent has written
        printf("Child read from shared memory: \"%s\"\n", shm_ptr);
        shmdt(shm_ptr);
        sem_close(sem);
        exit(0);
    } else {
        // --- PARENT PROCESS: writes to shared memory --
        char *shm_ptr = (char *)shmat(shmid, NULL, 0);
        if (shm_ptr == (char *)-1) { perror("shmat parent"); exit(1); }
        const char *message = "Hello from parent process!";
        strncpy(shm_ptr, message, SHM_SIZE);
        printf("Parent wrote to shared memory: \"%s\"\n", message);
        sem_post(sem); // signal child to read
        wait(NULL); // wait for child to finish
        // 5. Cleanup
        shmdt(shm_ptr);
        shmctl(shmid, IPC_RMID, NULL);
        sem_close(sem);
        sem_unlink(SEM_NAME);
        printf("Shared memory cleaned up.\n");
    }
    return 0;
}