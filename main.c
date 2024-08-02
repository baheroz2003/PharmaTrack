#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_SIZE 100
#define MAX_ITEMS 100

// Structure to represent an inventory item
typedef struct InventoryItem {
    int product_id;
    char name[50];
    int quantity;
    float price;
    char expiration_date[20];
    struct InventoryItem *next;
} InventoryItem;

// Structure to represent a customer order
typedef struct CustomerOrder {
    char name[50];
    int product_id;
    int quantity;
    int urgency; // Priority of the order
    struct CustomerOrder *next;
} CustomerOrder;

// Global variable to generate product IDs
int next_product_id = 1;

// Hash table to store inventory items
InventoryItem *hashTable[HASH_SIZE] = {NULL};

// Function prototypes
void addItem(InventoryItem **inventory);
void displayInventory();
void placeOrder(CustomerOrder **queue);
void processOrders(CustomerOrder **queue);

int main() {
    CustomerOrder *orderQueue = NULL;
    int choice;

    do {
        printf("\nMedical Store Inventory Management System\n");
        printf("1. Add Item\n");
        printf("2. Display Inventory\n");
        printf("3. Place Order\n");
        printf("4. Process Orders\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                addItem(hashTable);
                break;
            case 2:
                displayInventory();
                break;
            case 3:
                placeOrder(&orderQueue);
                break;
            case 4:
                processOrders(&orderQueue);
                break;
            case 5:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 5);

    // Free allocated memory before exiting
    for (int i = 0; i < HASH_SIZE; i++) {
        while (hashTable[i] != NULL) {
            InventoryItem *temp = hashTable[i];
            hashTable[i] = hashTable[i]->next;
            free(temp);
        }
    }

    // Free order queue
    while (orderQueue != NULL) {
        CustomerOrder *temp = orderQueue;
        orderQueue = orderQueue->next;
        free(temp);
    }

    return 0;
}

// Function to add an item to the inventory using hashing
void addItem(InventoryItem **inventory) {
    InventoryItem *newItem = (InventoryItem *)malloc(sizeof(InventoryItem));
    if (newItem == NULL) {
        printf("Memory allocation failed. Cannot add item.\n");
        return;
    }

    newItem->product_id = next_product_id++;
    printf("Enter item name: ");
    scanf("%s", newItem->name);
    printf("Enter quantity: ");
    scanf("%d", &(newItem->quantity));
    printf("Enter price: ");
    scanf("%f", &(newItem->price));
    printf("Enter expiration date: ");
    scanf("%s", newItem->expiration_date);

    // Calculate hash value
    unsigned int hash = newItem->product_id % HASH_SIZE;

    newItem->next = inventory[hash];
    inventory[hash] = newItem;

    printf("Item added successfully. Product ID: %d\n", newItem->product_id);
}

// Function to display the current inventory
void displayInventory() {
    printf("\nCurrent Inventory:\n");
    printf("-----------------------------------------------------------\n");
    printf("%-10s %-20s %-10s %-10s %-20s\n", "ID", "Name", "Quantity", "Price", "Expiration Date");
    printf("-----------------------------------------------------------\n");

    for (int i = 0; i < HASH_SIZE; i++) {
        InventoryItem *current = hashTable[i];
        while (current != NULL) {
            printf("%-10d %-20s %-10d %-10.2f %-20s\n", current->product_id, current->name, current->quantity, current->price, current->expiration_date);
            current = current->next;
        }
    }
}

// Function to place an order in the priority queue
// Function to place an order in the priority queue
void placeOrder(CustomerOrder **queue) {
    CustomerOrder *newOrder = (CustomerOrder *)malloc(sizeof(CustomerOrder));
    if (newOrder == NULL) {
        printf("Memory allocation failed. Cannot place order.\n");
        return;
    }

    printf("Enter customer name: ");
    scanf("%s", newOrder->name);
    printf("Enter product ID: ");
    scanf("%d", &(newOrder->product_id));
    printf("Enter quantity: ");
    scanf("%d", &(newOrder->quantity));
    printf("Enter order urgency (1 - Low, 2 - Medium, 3 - High): ");
    scanf("%d", &(newOrder->urgency));

    // Find the item in the inventory and check if requested quantity is available
    int productId = newOrder->product_id;
    int requestedQuantity = newOrder->quantity;
    InventoryItem *item = NULL;
    for (int i = 0; i < HASH_SIZE; i++) {
        InventoryItem *current = hashTable[i];
        while (current != NULL) {
            if (current->product_id == productId) {
                item = current;
                break;
            }
            current = current->next;
        }
        if (item != NULL)
            break;
    }

    if (item == NULL) {
        printf("Product with ID %d not found in inventory. Order cannot be placed.\n", productId);
        free(newOrder);
        return;
    }

    if (requestedQuantity > item->quantity) {
        printf("Requested quantity exceeds available quantity. Order cannot be placed.\n");
        free(newOrder);
        return;
    }

    // Insert order into the priority queue based on urgency
    if (*queue == NULL || newOrder->urgency > (*queue)->urgency) {
        newOrder->next = *queue;
        *queue = newOrder;
    } else {
        CustomerOrder *current = *queue;
        while (current->next != NULL && current->next->urgency >= newOrder->urgency) {
            current = current->next;
        }
        newOrder->next = current->next;
        current->next = newOrder;
    }

    printf("Order placed successfully.\n");
}

void processOrders(CustomerOrder **queue) {
    if (*queue == NULL) {
        printf("No orders to process.\n");
        return;
    }

    CustomerOrder *processedOrder = *queue;
    *queue = (*queue)->next;
    printf("Processing order for customer: %s\n", processedOrder->name);

    // Compute the hash value of the product ID
    int hashIndex = processedOrder->product_id % HASH_SIZE;

    // Search in the bucket corresponding to the hash index
    InventoryItem *current = hashTable[hashIndex];
    while (current != NULL) {
        if (current->product_id == processedOrder->product_id) {
            if (current->quantity < processedOrder->quantity) {
                printf("Insufficient quantity in inventory.\n");
                free(processedOrder);
                return;
            } else {
                current->quantity -= processedOrder->quantity;
                printf("Order processed successfully.\n");
                free(processedOrder);
                return;
            }
        }
        current = current->next;
    }

    printf("Product with ID %d not found in inventory.\n", processedOrder->product_id);
    free(processedOrder);
}
