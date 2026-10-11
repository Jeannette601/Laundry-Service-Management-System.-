
/*
 * Project: Laundry Service Management System
 * Student Registration Number: 30616/2026
 * Course: Data Structures using C Programming
 *
 * Features:
 * - Add, display, search and delete laundry records
 * - Dynamic memory allocation using malloc()
 * - Linked list data structure
 * - File persistence using laundry_data.txt
 * - Menu controlled by switch-case
 * - Memory released before program exit
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATA_FILE "laundry_data.txt"

typedef struct Laundry {
    int id;
    char customer[60];
    char service[60];
    int clothes;
    float price;
    struct Laundry *next;
} Laundry;

Laundry *head = NULL;
int nextId = 1;

/* Remove newline from text input */
void removeNewline(char *text) {
    text[strcspn(text, "\n")] = '\0';
}

/* Read a line of text safely */
void readText(const char *prompt, char *text, size_t size) {
    printf("%s", prompt);
    if (fgets(text, (int)size, stdin) != NULL) {
        removeNewline(text);
    } else {
        text[0] = '\0';
    }
}

/* Find a record by ID */
Laundry *findRecord(int id) {
    Laundry *current = head;
    while (current != NULL) {
        if (current->id == id)
            return current;
        current = current->next;
    }
    return NULL;
}

/* Save records to a file */
void saveRecords(void) {
    FILE *file = fopen(DATA_FILE, "w");
    Laundry *current = head;

    if (file == NULL) {
        printf("Error: Could not save records.\n");
        return;
    }

    while (current != NULL) {
        fprintf(file, "%d|%s|%s|%d|%.2f\n",
                current->id, current->customer,
                current->service, current->clothes,
                current->price);
        current = current->next;
    }

    fclose(file);
}

/* Load saved records at startup */
void loadRecords(void) {
    FILE *file = fopen(DATA_FILE, "r");
    int id, clothes;
    float price;
    char customer[60], service[60];
    Laundry *record, *tail = NULL;

    if (file == NULL)
        return;

    while (fscanf(file, "%d|%59[^|]|%59[^|]|%d|%f\n",
                  &id, customer, service, &clothes,
                  &price) == 5) {
        record = malloc(sizeof(Laundry));

        if (record == NULL) {
            printf("Memory allocation failed.\n");
            break;
        }

        record->id = id;
        strcpy(record->customer, customer);
        strcpy(record->service, service);
        record->clothes = clothes;
        record->price = price;
        record->next = NULL;

        if (head == NULL)
            head = record;
        else
            tail->next = record;

        tail = record;

        if (id >= nextId)
            nextId = id + 1;
    }

    fclose(file);
}

/* Add a new laundry order */
void addRecord(void) {
    Laundry *record = malloc(sizeof(Laundry));
    if (record == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    record->id = nextId++;

    readText("Customer name: ", record->customer,
             sizeof(record->customer));
    readText("Laundry service: ", record->service,
             sizeof(record->service));

    printf("Number of clothes: ");
    if (scanf("%d", &record->clothes) != 1 ||
        record->clothes <= 0) {
        printf("Invalid number of clothes.\n");
        while (getchar() != '\n');
        free(record);
        return;
    }

    printf("Total price: ");
    if (scanf("%f", &record->price) != 1 ||
        record->price < 0) {
        printf("Invalid price.\n");
        while (getchar() != '\n');
        free(record);
        return;
    }
    while (getchar() != '\n');

    record->next = head;
    head = record;

    saveRecords();
    printf("Order added successfully. ID: %d\n", record->id);
}

/* Display all orders */
void displayRecords(void) {
    Laundry *current = head;

    if (current == NULL) {
        printf("No laundry orders found.\n");
        return;
    }

    printf("\n--- LAUNDRY ORDERS ---\n");
    while (current != NULL) {
        printf("ID: %d | Customer: %s | Service: %s\n",
               current->id, current->customer,
               current->service);
        printf("Clothes: %d | Price: %.2f\n",
               current->clothes, current->price);
        printf("-----------------------------\n");
        current = current->next;
    }
}

/* Search for an order */
void searchRecord(void) {
    int id;
    printf("Enter order ID: ");

    if (scanf("%d", &id) != 1) {
        printf("Invalid ID.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    Laundry *record = findRecord(id);
    if (record == NULL) {
        printf("Order not found.\n");
    } else {
        printf("Customer: %s\n", record->customer);
        printf("Service: %s\n", record->service);
        printf("Clothes: %d\n", record->clothes);
        printf("Price: %.2f\n", record->price);
    }
}

/* Delete an order */
void deleteRecord(void) {
    int id;
    Laundry *current = head, *previous = NULL;

    printf("Enter order ID to delete: ");
    if (scanf("%d", &id) != 1) {
        printf("Invalid ID.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    while (current != NULL && current->id != id) {
        previous = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("Order not found.\n");
        return;
    }

    if (previous == NULL)
        head = current->next;
    else
        previous->next = current->next;

    free(current);
    saveRecords();
    printf("Order deleted successfully.\n");
}

/* Release all dynamically allocated memory */
void freeRecords(void) {
    Laundry *current = head;

    while (current != NULL) {
        Laundry *temporary = current;
        current = current->next;
        free(temporary);
    }

    head = NULL;
}

int main(void) {
    int choice;

    loadRecords();

    do {
        printf("\n================================\n");
        printf(" LAUNDRY SERVICE MANAGEMENT\n");
        printf("================================\n");
        printf("1. Add laundry order\n");
        printf("2. Display all orders\n");
        printf("3. Search order\n");
        printf("4. Delete order\n");
        printf("5. Save records\n");
        printf("0. Save and Exit\n");
        printf("Choose an option: ");

        if (scanf("%d", &choice) != 1) {
            printf("Please enter a valid number.\n");
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');

        switch (choice) {
            case 1:
                addRecord();
                break;
            case 2:
                displayRecords();
                break;
            case 3:
                searchRecord();
                break;
            case 4:
                deleteRecord();
                break;
            case 5:
                saveRecords();
                printf("Records saved.\n");
                break;
            case 0:
                saveRecords();
                printf("Records saved. Goodbye!\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 0);

    freeRecords();
    return 0;
}
