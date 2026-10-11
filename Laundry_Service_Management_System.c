#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATA_FILE "laundry_data.txt"
#define NAME_LEN 60
#define CLOTHES_LEN 80
#define STATUS_LEN 20

typedef struct LaundryOrder {
    int orderId;
    char customerName[NAME_LEN];
    char clothesDescription[CLOTHES_LEN];
    int quantity;
    float price;
    char status[STATUS_LEN];
    struct LaundryOrder *next;
} LaundryOrder;

static LaundryOrder *head = NULL;
static int nextOrderId = 1001;

void removeNewline(char *text) {
    text[strcspn(text, "\n")] = '\0';
}

void readLine(const char *prompt, char *buffer, size_t size) {
    printf("%s", prompt);
    if (fgets(buffer, (int)size, stdin) != NULL) {
        removeNewline(buffer);
    } else {
        buffer[0] = '\0';
    }
}

int readInt(const char *prompt) {
    char buffer[100], *end;
    long value;
    while (1) {
        readLine(prompt, buffer, sizeof(buffer));
        value = strtol(buffer, &end, 10);
        if (end != buffer && *end == '\0') return (int)value;
        printf("Please enter a valid whole number.\n");
    }
}

float readFloat(const char *prompt) {
    char buffer[100], *end;
    float value;
    while (1) {
        readLine(prompt, buffer, sizeof(buffer));
        value = strtof(buffer, &end);
        if (end != buffer && *end == '\0' && value >= 0) return value;
        printf("Please enter a valid amount (0 or more).\n");
    }
}

LaundryOrder *findOrder(int id) {
    LaundryOrder *current = head;
    while (current != NULL) {
        if (current->orderId == id) return current;
        current = current->next;
    }
    return NULL;
}

void saveOrders(void) {
    FILE *file = fopen(DATA_FILE, "w");
    LaundryOrder *current = head;

    if (file == NULL) {
        printf("Warning: Could not save data to %s.\n", DATA_FILE);
        return;
    }

    while (current != NULL) {
        /* Use a simple delimiter format; avoid | in user-entered text. */
        fprintf(file, "%d|%s|%s|%d|%.2f|%s\n",
                current->orderId, current->customerName,
                current->clothesDescription, current->quantity,
                current->price, current->status);
        current = current->next;
    }
    fclose(file);
}

void loadOrders(void) {
    FILE *file = fopen(DATA_FILE, "r");
    char line[300];

    if (file == NULL) return; /* First run: no saved file yet. */

    while (fgets(line, sizeof(line), file) != NULL) {
        LaundryOrder *node = malloc(sizeof(LaundryOrder));
        if (node == NULL) {
            printf("Not enough memory to load all records.\n");
            break;
        }

        if (sscanf(line, "%d|%59[^|]|%79[^|]|%d|%f|%19[^\n]",
                   &node->orderId, node->customerName,
                   node->clothesDescription, &node->quantity,
                   &node->price, node->status) == 6) {
            node->next = NULL;
            if (head == NULL) {
                head = node;
            } else {
                LaundryOrder *tail = head;
                while (tail->next != NULL) tail = tail->next;
                tail->next = node;
            }
            if (node->orderId >= nextOrderId) nextOrderId = node->orderId + 1;
        } else {
            free(node);
        }
    }
    fclose(file);
}

void addOrder(void) {
    LaundryOrder *node = malloc(sizeof(LaundryOrder));
    if (node == NULL) {
        printf("Error: Memory allocation failed. Order was not added.\n");
        return;
    }

    node->orderId = nextOrderId++;
    readLine("Customer name: ", node->customerName, sizeof(node->customerName));
    readLine("Clothes description: ", node->clothesDescription,
             sizeof(node->clothesDescription));
    node->quantity = readInt("Number of items: ");
    while (node->quantity <= 0) {
        printf("Quantity must be greater than zero.\n");
        node->quantity = readInt("Number of items: ");
    }
    node->price = readFloat("Total price (RWF): ");
    strcpy(node->status, "Received");
    node->next = NULL;

    if (head == NULL) {
        head = node;
    } else {
        LaundryOrder *tail = head;
        while (tail->next != NULL) tail = tail->next;
        tail->next = node;
    }

    saveOrders();
    printf("\nOrder registered successfully. Order ID: %d\n", node->orderId);
}

void listOrders(void) {
    LaundryOrder *current = head;
    if (current == NULL) {
        printf("\nNo laundry orders have been registered yet.\n");
        return;
    }

    printf("\n%-8s %-20s %-22s %-8s %-12s %-12s\n",
           "ID", "CUSTOMER", "CLOTHES", "QTY", "PRICE(RWF)", "STATUS");
    printf("--------------------------------------------------------------------------------------\n");
    while (current != NULL) {
        printf("%-8d %-20.20s %-22.22s %-8d %-12.2f %-12s\n",
               current->orderId, current->customerName,
               current->clothesDescription, current->quantity,
               current->price, current->status);
        current = current->next;
    }
}

void searchOrder(void) {
    int id = readInt("Enter order ID to search: ");
    LaundryOrder *order = findOrder(id);
    if (order == NULL) {
        printf("No order found with ID %d.\n", id);
        return;
    }

    printf("\nOrder ID: %d\nCustomer: %s\nClothes: %s\nQuantity: %d\n"
           "Price: RWF %.2f\nStatus: %s\n",
           order->orderId, order->customerName, order->clothesDescription,
           order->quantity, order->price, order->status);
}

void updateStatus(void) {
    int id = readInt("Enter order ID to update: ");
    LaundryOrder *order = findOrder(id);
    int choice;

    if (order == NULL) {
        printf("No order found with ID %d.\n", id);
        return;
    }

    printf("1. Received\n2. Washing\n3. Ready for collection\n4. Collected\n");
    choice = readInt("Choose new status: ");
    switch (choice) {
        case 1: strcpy(order->status, "Received"); break;
        case 2: strcpy(order->status, "Washing"); break;
        case 3: strcpy(order->status, "Ready"); break;
        case 4: strcpy(order->status, "Collected"); break;
        default:
            printf("Invalid status choice. No change made.\n");
            return;
    }

    saveOrders();
    printf("Order status updated successfully.\n");
}

void deleteOrder(void) {
    int id = readInt("Enter order ID to delete: ");
    LaundryOrder *current = head, *previous = NULL;

    while (current != NULL && current->orderId != id) {
        previous = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("No order found with ID %d.\n", id);
        return;
    }

    if (previous == NULL) head = current->next;
    else previous->next = current->next;

    free(current);
    saveOrders();
    printf("Order deleted successfully.\n");
}

void showMenu(void) {
    printf("\n============================================\n");
    printf("       LAUNDRY SERVICE MANAGEMENT SYSTEM\n");
    printf("============================================\n");
    printf("1. Register a laundry order\n");
    printf("2. View all orders\n");
    printf("3. Search for an order\n");
    printf("4. Update order status\n");
    printf("5. Delete an order\n");
    printf("6. Save data\n");
    printf("0. Exit\n");
    printf("--------------------------------------------\n");
}

void freeAllOrders(void) {
    LaundryOrder *current = head;
    while (current != NULL) {
        LaundryOrder *next = current->next;
        free(current);
        current = next;
    }
    head = NULL;
}

int main(void) {
    int choice;
    loadOrders();

    printf("Welcome to the Laundry Service Management System.\n");
    printf("Institution: UNILAK | Course: Data Structure\n");
    printf("Registration Number: 30616/2026\n");

    do {
        showMenu();
        choice = readInt("Enter your choice: ");
        switch (choice) {
            case 1: addOrder(); break;
            case 2: listOrders(); break;
            case 3: searchOrder(); break;
            case 4: updateStatus(); break;
            case 5: deleteOrder(); break;
            case 6: saveOrders(); printf("Data saved.\n"); break;
            case 0:
                saveOrders();
                printf("Data saved. Goodbye!\n");
                break;
            default:
                printf("Invalid choice. Please select a menu option.\n");
        }
    } while (choice != 0);

    freeAllOrders();
    return 0;
}
