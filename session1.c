//  Mutex Lock Demonstration
#include <stdio.h>
#include <pthread.h>
#define NUM_THREADS 10
#define NUM_INCREMENTS 1000
long counter_with_mutex = 0;
long counter_without_mutex = 0;
pthread_mutex_t lock;
void *increment_with_mutex(void *arg) {
    for (int i = 0; i < NUM_INCREMENTS; i++) {
        pthread_mutex_lock(&lock);
        counter_with_mutex++;
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}
void *increment_without_mutex(void *arg) {
    for (int i = 0; i < NUM_INCREMENTS; i++) {
        counter_without_mutex++;
    }
    return NULL;
}
int main() {
    pthread_t threads[NUM_THREADS];
    pthread_mutex_init(&lock, NULL);
    // WITHOUT mutex
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_create(&threads[i], NULL, increment_without_mutex, NULL);
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);
    printf("Without mutex: %ld (expected %d)\n", counter_without_mutex, NUM_THREADS *
NUM_INCREMENTS);
    // WITH mutex
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_create(&threads[i], NULL, increment_with_mutex, NULL);
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);
    printf("With mutex: %ld (expected %d)\n", counter_with_mutex, NUM_THREADS * NUM_INCREMENTS);
    pthread_mutex_destroy(&lock);
    return 0;
}