// Semaphore Implementation in C
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#define NUM_THREADS 6
#define NUM_INCREMENTS 1000
#define MAX_CONCURRENT 3 // counting semaphore allows 3 threads simultaneously
long counter_mutex = 0;
long counter_sem = 0;
pthread_mutex_t mutex_lock;
sem_t binary_sem;
sem_t counting_sem;
void *increment_with_mutex(void *arg) {
    for (int i = 0; i < NUM_INCREMENTS; i++) {
        pthread_mutex_lock(&mutex_lock);
        counter_mutex++;
        pthread_mutex_unlock(&mutex_lock);
    }
    return NULL;
}
void *increment_with_semaphore(void *arg) {
    for (int i = 0; i < NUM_INCREMENTS; i++) {
        sem_wait(&binary_sem);
        counter_sem++;
        sem_post(&binary_sem);
    }
    return NULL;
}
void *counting_sem_task(void *arg) {
    int id = *(int *)arg;
    sem_wait(&counting_sem);
    printf("Thread %d entered critical section\n", id);
    sleep(1); // simulate work
    printf("Thread %d leaving critical section\n", id);
    sem_post(&counting_sem);
    return NULL;
}
int main() {
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];
    pthread_mutex_init(&mutex_lock, NULL);
    sem_init(&binary_sem, 0, 1);
    sem_init(&counting_sem, 0, MAX_CONCURRENT);
    // --- Compare mutex vs semaphore for counter --
    clock_t start, end;
    start = clock();
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_create(&threads[i], NULL, increment_with_mutex, NULL);
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);
    end = clock();
    printf("Mutex counter: %ld | Time: %lf ms\n", counter_mutex,
           (double)(end - start) / CLOCKS_PER_SEC * 1000);
    start = clock();
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_create(&threads[i], NULL, increment_with_semaphore, NULL);
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);
    end = clock();
    printf("Semaphore counter: %ld | Time: %lf ms\n", counter_sem,
           (double)(end - start) / CLOCKS_PER_SEC * 1000);
    // --- Counting semaphore: allow 3 threads simultaneously --
    printf("\nCounting semaphore (max %d concurrent threads):\n", MAX_CONCURRENT);
    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i + 1;
        pthread_create(&threads[i], NULL, counting_sem_task, &ids[i]);
    }
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);
    pthread_mutex_destroy(&mutex_lock);
    sem_destroy(&binary_sem);
    sem_destroy(&counting_sem);
    return 0;
}