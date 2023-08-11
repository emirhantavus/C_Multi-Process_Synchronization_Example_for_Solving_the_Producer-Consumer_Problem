#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_THREADS 10

pthread_mutex_t mutex;
pthread_cond_t cond;   
int current_thread = 1; 

void *thread_function(void *arg) {
    int thread_num = *((int *)arg);
    
    pthread_mutex_lock(&mutex);
    
    while (current_thread != thread_num) {
        pthread_cond_wait(&cond, &mutex);
    }
    sleep(1);
    printf("%d.yazildi \n", thread_num);
    fflush(stdout);

    current_thread++;
    if (current_thread > NUM_THREADS) {
        current_thread = 1;
    }
    
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main() {
    int pid;
    int pd[2];
    pthread_t threads[NUM_THREADS];
    int thread_args[NUM_THREADS];

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL); 

    int ret = pipe(pd);
    if (ret < 0) {
        printf("pipe olusturulamadi.");
        exit(1);
    }

    while (1) {
        pid = fork();
        if (pid < 0) {
            printf("fork olusturulamadi.");
        } else if (pid > 0) {
            wait(NULL);
            char oku[10];
            close(pd[1]);
            read(pd[0], oku, 8);
        } else if (pid == 0) {
            close(pd[0]);

            for (int i = 1; i <= NUM_THREADS; ++i) {
                thread_args[i-1] = i;
                pthread_create(&threads[i-1], NULL, thread_function, &thread_args[i-1]);
            }

            for (int i = 0; i < NUM_THREADS; ++i) {
                pthread_join(threads[i], NULL);
            }

            exit(0);
        }

        sleep(5);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    return 0;
}
