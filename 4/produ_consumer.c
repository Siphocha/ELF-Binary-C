#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define MAX_QUEUE_SIZE 8
#define PREP_TIME 4
#define SERVE_TIME 3

//Shared data struct for instances
typedef struct {
    int drinks[MAX_QUEUE_SIZE];
    int front;
    int rear;
    int count;
    int total_prepared;
    int total_served;
    
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} OrderQueue;

OrderQueue queue;

//Shared queue of threads
void init_queue(OrderQueue *q) {
    q->front = 0;
    q->rear = 0;
    q->count = 0;
    q->total_prepared = 0;
    q->total_served = 0;
    
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    pthread_cond_init(&q->not_full, NULL);
}

//Barista thread function
void* barista_thread(void* arg) {
    printf("Barista started work! Preparing drinks.\n");
    
    while (1) {
        sleep(PREP_TIME);
        
        pthread_mutex_lock(&queue.mutex);
        
        //If queue is full then pause
        while (queue.count == MAX_QUEUE_SIZE) {
            printf("BARISTA PAUSED - Queue full (%d/drinks waiting)\n", queue.count);
            pthread_cond_wait(&queue.not_full, &queue.mutex);
            printf("BARISTA RESUMED \n");
        }
        
        //Adding drink to queue
        int drink_id = ++queue.total_prepared;
        queue.drinks[queue.rear] = drink_id;
        queue.rear = (queue.rear + 1) % MAX_QUEUE_SIZE;
        queue.count++;
        
        printf("Barista prepared drink #%d | Queue size is: %d/8\n", 
               drink_id, queue.count);
        
        //Let it be known queue is not empty
        pthread_cond_signal(&queue.not_empty);
        pthread_mutex_unlock(&queue.mutex);
    }
    
    return NULL;
}

//Waiter thread
void* waiter_thread(void* arg) {
    printf("Waiter started work!\n");
    
    while (1) {
        pthread_mutex_lock(&queue.mutex);
        
        // Wait if queue is empty - waiter must wait
        while (queue.count == 0) {
            printf("WAITER WAITING, no drinks available\n");
            pthread_cond_wait(&queue.not_empty, &queue.mutex);
            printf("WAITER IS UP, new drink ready!\n");
        }
        
        //Removed drink from queue
        int drink_id = queue.drinks[queue.front];
        queue.front = (queue.front + 1) % MAX_QUEUE_SIZE;
        queue.count--;
        queue.total_served++;
        
        printf("Waiter picked up drink #%d | Queue size: %d/8\n", 
               drink_id, queue.count);
        
        pthread_cond_signal(&queue.not_full);
        pthread_mutex_unlock(&queue.mutex);
        
        sleep(SERVE_TIME);
        printf("Drink #%d served to customer!\n", drink_id);
    }
    
    return NULL;
}

//Monitor thread to display queue status 
void* monitor_thread(void* arg) {
    while (1) {
        sleep(5); // Report every 5 seconds...arbitrary but can be shorter.
        pthread_mutex_lock(&queue.mutex);
        printf("\nSYSTEM STATUS: %d drinks in queue | Total prepared: %d | Total served: %d\n", 
               queue.count, queue.total_prepared, queue.total_served);
        pthread_mutex_unlock(&queue.mutex);
    }
    return NULL;
}

int main() {
    pthread_t barista, waiter, monitor;
    
    printf("Coffee Shop Simulation Started!\n");
    printf("\n");
    printf("Barista: Prepares 1 drink every %d seconds\n", PREP_TIME);
    printf("Waiter:  Serves 1 drink every %d seconds\n", SERVE_TIME);
    printf("Queue:   Max %d drinks waiting\n", MAX_QUEUE_SIZE);
    printf("\n\n");
    
    //Initialising shared queue
    init_queue(&queue);
    
    //Creating threads
    if (pthread_create(&barista, NULL, barista_thread, NULL) != 0) {
        perror("Failing to create barista thread");
        return 1;
    }
    
    if (pthread_create(&waiter, NULL, waiter_thread, NULL) != 0) {
        perror("Failing to create waiter thread");
        return 1;
    }
    
    if (pthread_create(&monitor, NULL, monitor_thread, NULL) != 0) {
        perror("Failing to create monitor thread");
        return 1;
    }
    
    //Simulation runtime
    sleep(20);
    
    printf("Coffee Shop Simulation Done!\n");
    printf("Final Stats: %d drinks prepared, %d drinks served\n", 
           queue.total_prepared, queue.total_served);
    
    //Basic cleanup
    pthread_cancel(barista);
    pthread_cancel(waiter);
    pthread_cancel(monitor);
    
    pthread_mutex_destroy(&queue.mutex);
    pthread_cond_destroy(&queue.not_empty);
    pthread_cond_destroy(&queue.not_full);
    
    return 0;
}