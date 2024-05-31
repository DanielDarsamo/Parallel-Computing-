#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUM_SELF_CHECKOUT_LANES 4
#define NUM_CASHIERS 3
#define FAST_LANE_ID 3

typedef struct {
    int lane_id;
    int customer_id;
} CustomerInfo;

typedef struct {
    int num_customers;
    int lane_id;
} LaneLoad;

// Function to get the current timestamp
void get_current_time(char* buffer) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", t);
}

// Function to simulate a customer using a self-checkout lane
void* self_checkout(void* arg) {
    CustomerInfo* info = (CustomerInfo*)arg;
    int checkout_time = rand() % 5 + 1;  // Simulate checkout time between 1 to 5 seconds
    char start_time[20], end_time[20];
    get_current_time(start_time);
    printf("Customer %d at self-checkout lane %d starts checkout at %s.\n", info->customer_id, info->lane_id, start_time);
    sleep(checkout_time);
    get_current_time(end_time);
    printf("Customer %d at self-checkout lane %d completes checkout at %s.\n", info->customer_id, info->lane_id, end_time);
    free(info);
    return NULL;
}

// Function to simulate a customer being processed by a cashier
void* cashier_checkout(void* arg) {
    CustomerInfo* info = (CustomerInfo*)arg;
    int checkout_time = rand() % 5 + 1;  // Simulate checkout time between 1 to 5 seconds
    char start_time[20], end_time[20];
    get_current_time(start_time);
    printf("Customer %d at cashier lane %d starts checkout at %s.\n", info->customer_id, info->lane_id, start_time);
    sleep(checkout_time);
    get_current_time(end_time);
    printf("Customer %d at cashier lane %d completes checkout at %s.\n", info->customer_id, info->lane_id, end_time);
    free(info);
    return NULL;
}

// Function to print the number of customers in each lane
void print_lane_info(LaneLoad* lanes, int num_lanes) {
    printf("\nNumber of customers in each lane:\n");
    for (int i = 0; i < num_lanes; i++) {
        printf("Lane %d: %d customers\n", lanes[i].lane_id, lanes[i].num_customers);
    }
    printf("\n");
}

// Function to distribute customers among lanes and create threads
void distribute_customers(int num_lanes, int total_customers, void* (*checkout_func)(void*)) {
    pthread_t* threads = malloc(total_customers * sizeof(pthread_t));
    LaneLoad* lanes = malloc(num_lanes * sizeof(LaneLoad));
    int thread_index = 0;

    for (int i = 0; i < num_lanes; ++i) {
        lanes[i].num_customers = 0;
        lanes[i].lane_id = i + 1;
    }

    for (int customer = 0; customer < total_customers; ++customer) {
        int lane = customer % num_lanes;
        CustomerInfo* info = malloc(sizeof(CustomerInfo));
        info->lane_id = lanes[lane].lane_id;
        info->customer_id = customer + 1;
        lanes[lane].num_customers++;

        if (pthread_create(&threads[thread_index], NULL, checkout_func, info)) {
            fprintf(stderr, "Error creating thread\n");
            return;
        }
        thread_index++;
    }

    // Wait for all threads to complete
    for (int i = 0; i < total_customers; ++i) {
        if (pthread_join(threads[i], NULL)) {
            fprintf(stderr, "Error joining thread\n");
            return;
        }
    }

    // Print the number of customers in each lane
    print_lane_info(lanes, num_lanes);

    free(threads);
    free(lanes);
}

int main() {
    srand(time(NULL));  // Seed the random number generator

    int num_self_checkout_customers, num_fast_lane_customers, num_normal_lane_customers;

    // Take input for the number of customers
    printf("Enter the number of customers for self-checkout lanes: ");
    scanf("%d", &num_self_checkout_customers);
    printf("Enter the number of customers for the fast lane (15 items or less): ");
    scanf("%d", &num_fast_lane_customers);
    printf("Enter the number of customers for the normal lanes: ");
    scanf("%d", &num_normal_lane_customers);

    // Process the self-checkout lanes
    printf("Processing self-checkout lanes with one cashier...\n");
    distribute_customers(NUM_SELF_CHECKOUT_LANES, num_self_checkout_customers, self_checkout);

    // Process the cashier lanes, including the fast lane
    printf("Processing cashier lanes...\n");
    distribute_customers(NUM_CASHIERS, num_fast_lane_customers + num_normal_lane_customers, cashier_checkout);

    return 0;
}
