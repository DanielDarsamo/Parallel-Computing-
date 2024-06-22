#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

#define NUM_SELF_CHECKOUT_LANES 4
#define NUM_CASHIERS 3
#define FAST_LANE_ID 3
#define THREAD_POOL_SIZE 100  // Define a fixed number of threads in the thread pool

typedef struct {
    int lane_id;
    int customer_id;
    int priority;
    time_t timestamp;
} CustomerInfo;

typedef struct {
    int num_customers;
    int lane_id;
    sem_t lane_sem;
} LaneLoad;

pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
LaneLoad lanes[NUM_SELF_CHECKOUT_LANES + NUM_CASHIERS];
pthread_t thread_pool[THREAD_POOL_SIZE];
sem_t thread_pool_sem;

void get_current_time(char* buffer) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", t);
}

void* checkout(void* arg) {
    CustomerInfo* info = (CustomerInfo*)arg;
    char start_time[20], end_time[20];
    get_current_time(start_time);
    printf("Customer %d at lane %d starts checkout at %s.\n", info->customer_id, info->lane_id, start_time);
    sleep(rand() % 5 + 1);
    get_current_time(end_time);
    printf("Customer %d at lane %d completes checkout at %s.\n", info->customer_id, info->lane_id, end_time);
    free(info);
    sem_post(&thread_pool_sem);  // Signal that a thread in the pool has completed its task
    return NULL;
}

void distribute_customers(int num_lanes, int total_customers) {
    int thread_index = 0;

    for (int i = 0; i < num_lanes; ++i) {
        lanes[i].num_customers = 0;
        lanes[i].lane_id = i + 1;
        sem_init(&lanes[i].lane_sem, 0, 1);
    }

    for (int customer = 0; customer < total_customers; ++customer) {
        int lane = rand() % num_lanes;
        sem_wait(&lanes[lane].lane_sem);
        CustomerInfo* info = malloc(sizeof(CustomerInfo));
        info->lane_id = lanes[lane].lane_id;
        info->customer_id = customer + 1;
        lanes[lane].num_customers++;

        sem_wait(&thread_pool_sem);  // Wait for an available thread in the pool
        if (pthread_create(&thread_pool[thread_index], NULL, checkout, info)) {
            fprintf(stderr, "Error creating thread\n");
            return;
        }
        thread_index = (thread_index + 1) % THREAD_POOL_SIZE;  // Use modulo to cycle through the thread pool
        sem_post(&lanes[lane].lane_sem);
    }

    // Wait for all threads in the pool to complete
    for (int i = 0; i < THREAD_POOL_SIZE; ++i) {
        pthread_join(thread_pool[i], NULL);
    }

    printf("\nNumber of customers in each lane:\n");
    for (int i = 0; i < num_lanes; i++) {
        printf("Lane %d: %d customers\n", lanes[i].lane_id, lanes[i].num_customers);
    }
    printf("\n");
}

int main() {
    srand(time(NULL));
    sem_init(&thread_pool_sem, 0, THREAD_POOL_SIZE);  // Initialize semaphore for the thread pool

    int num_self_checkout_customers, num_fast_lane_customers, num_normal_lane_customers;
    printf("Enter the number of customers for self-checkout lanes: ");
    scanf("%d", &num_self_checkout_customers);
    printf("Enter the number of customers for the fast lane (15 items or less): ");
    scanf("%d", &num_fast_lane_customers);
    printf("Enter the number of customers for the normal lanes: ");
    scanf("%d", &num_normal_lane_customers);

    printf("Processing self-checkout lanes with one cashier...\n");
    distribute_customers(NUM_SELF_CHECKOUT_LANES, num_self_checkout_customers);

    printf("Processing cashier lanes...\n");
    distribute_customers(NUM_CASHIERS, num_fast_lane_customers + num_normal_lane_customers);

    sem_destroy(&thread_pool_sem);  // Destroy semaphore after use
    return 0;
}
