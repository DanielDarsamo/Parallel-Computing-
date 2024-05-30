#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Define constants
#define NUM_SELF_CHECKOUT_LANES 4
#define NUM_CASHIERS 3
#define FAST_LANE_ID 3

// Structure to represent lane data
typedef struct {
    int lane_id;
    int num_customers;
} LaneData;

// Function to simulate a customer using a self-checkout lane
void* self_checkout(void* lane_data) {
    LaneData* data = (LaneData*)lane_data;
    int id = data->lane_id;
    for (int i = 0; i < data->num_customers; ++i) {
        int checkout_time = rand() % 5 + 1;  // Simulate checkout time between 1 to 5 seconds
        printf("Customer %d at self-checkout lane %d starts checkout.\n", i + 1, id);
        sleep(checkout_time);
        printf("Customer %d at self-checkout lane %d completes checkout.\n", i + 1, id);
    }
    free(lane_data);  // Free the allocated memory
    return NULL;
}

// Function to simulate a customer being processed by a cashier
void* cashier_checkout(void* lane_data) {
    LaneData* data = (LaneData*)lane_data;
    int id = data->lane_id;
    for (int i = 0; i < data->num_customers; ++i) {
        int checkout_time = rand() % 5 + 1;  // Simulate checkout time between 1 to 5 seconds
        printf("Customer %d at cashier lane %d starts checkout.\n", i + 1, id);
        sleep(checkout_time);
        printf("Customer %d at cashier lane %d completes checkout.\n", i + 1, id);
    }
    free(lane_data);  // Free the allocated memory
    return NULL;
}

// Function to create and manage cashier threads for self-checkout lanes
void process_self_checkouts(int customers_per_lane[NUM_SELF_CHECKOUT_LANES]) {
    pthread_t threads[NUM_SELF_CHECKOUT_LANES];
    for (int i = 0; i < NUM_SELF_CHECKOUT_LANES; ++i) {
        LaneData* data = malloc(sizeof(LaneData));
        data->lane_id = i + 1;
        data->num_customers = customers_per_lane[i];
        if (pthread_create(&threads[i], NULL, self_checkout, data)) {
            fprintf(stderr, "Error creating thread\n");
            return;
        }
    }

    // Wait for all threads to complete
    for (int i = 0; i < NUM_SELF_CHECKOUT_LANES; ++i) {
        if (pthread_join(threads[i], NULL)) {
            fprintf(stderr, "Error joining thread\n");
            return;
        }
    }
    printf("All self-checkout lanes processed their customers.\n");
}

// Function to create and manage cashier threads for regular lanes
void process_cashiers(int customers_per_lane[NUM_CASHIERS]) {
    pthread_t threads[NUM_CASHIERS];
    for (int i = 0; i < NUM_CASHIERS; ++i) {
        LaneData* data = malloc(sizeof(LaneData));
        data->lane_id = i + 1;
        data->num_customers = customers_per_lane[i];
        if (pthread_create(&threads[i], NULL, cashier_checkout, data)) {
            fprintf(stderr, "Error creating thread\n");
            return;
        }
    }

    // Wait for all threads to complete
    for (int i = 0; i < NUM_CASHIERS; ++i) {
        if (pthread_join(threads[i], NULL)) {
            fprintf(stderr, "Error joining thread\n");
            return;
        }
    }
    printf("All cashier lanes processed their customers.\n");
}

int main() {
    srand(time(NULL));  // Seed the random number generator

    // Input the number of customers
    int customers_self_checkout[NUM_SELF_CHECKOUT_LANES];
    int customers_cashiers[NUM_CASHIERS];

    printf("Enter the number of customers for each self-checkout lane:\n");
    for (int i = 0; i < NUM_SELF_CHECKOUT_LANES; ++i) {
        printf("Self-checkout lane %d: ", i + 1);
        scanf("%d", &customers_self_checkout[i]);
    }

    printf("Enter the number of customers for each cashier lane:\n");
    for (int i = 0; i < NUM_CASHIERS; ++i) {
        printf("Cashier lane %d: ", i + 1);
        scanf("%d", &customers_cashiers[i]);
    }

    // Process the self-checkout lanes
    printf("Processing self-checkout lanes with one cashier...\n");
    process_self_checkouts(customers_self_checkout);

    // Process the cashier lanes, including the fast lane
    printf("Processing cashier lanes...\n");
    process_cashiers(customers_cashiers);

    return 0;
}
