#include <stdio.h>

int main() {
    int num_cashiers;
    int customers_per_cashier;
    int hours_of_operation;

    // Input values
    printf("Enter the number of cashiers: ");
    scanf("%d", &num_cashiers);

    printf("Enter the number of customers each cashier can handle per hour: ");
    scanf("%d", &customers_per_cashier);

    printf("Enter the hours of operation: ");
    scanf("%d", &hours_of_operation);

    // Compute customer handling capacity
    int total_customers_handled = num_cashiers * customers_per_cashier * hours_of_operation;

    // Output result
    printf("Total customer handling capacity: %d customer-hours\n", total_customers_handled);

    return 0;
}

