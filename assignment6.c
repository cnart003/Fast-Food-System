#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// structure
// order structure from assignment
struct order {
    int orderID;
    char *customerName;
    struct items *orderItems;
    int itemCount;
};

// items structure from assignment
struct items {
    char *itemName;
    float price;
};

// array to store the menu
struct items menuItems[] = {
    {"burger", 4.99},
    {"sandwich", 4.49},
    {"tenders", 3.99},
    {"fries", 1.99},
    {"cookies", 1.00},
    {"soft drink", 0.89},
};

// array to store the total customer orders on file
struct order customerOrders[100];

// functions
void displayMenu(struct items *menuItems, int menuSize);
void inputOrder(struct items *menuItems, int menuSize, struct order *customerOrders, int *orderCount, int orderID);
void calculateBill(struct order *customerOrders, int orderCount);
void salesSummary(struct order *customerOrders, int orderCount);

// main
int main() {
    // variables
    int orderID = 1;
    int orderCount = 0;
    char customerName[50];
    char newCustomer;

    do {
        // call functions
        displayMenu(menuItems, sizeof(menuItems) / sizeof(menuItems[0]));
        inputOrder(menuItems, sizeof(menuItems) / sizeof(menuItems[0]), customerOrders, &orderCount, orderID);
        calculateBill(customerOrders, orderCount);
        salesSummary(customerOrders, orderCount);

        // checks if the user is a new customer
        printf("are you a new customer? (Y/N): ");
        scanf(" %c", &newCustomer);
        getchar();  

        // add a number for each new order
        orderID++;

        // clear the order for the next customer
        orderCount = 0;

    } while (newCustomer == 'Y' || newCustomer == 'y');

    // display final sales summary
    printf("\nfinal sales summary:\n");
    salesSummary(customerOrders, orderCount);

    return 0;
}


// display the menu to the customer function
void displayMenu(struct items *menuItems, int menuSize) {
    printf("here is our menu!:\n");
    printf("\n---------------------\n");
    for (int i = 0; i < menuSize; i++) {
        printf("%d. %s - $%.2f\n", i + 1, menuItems[i].itemName, menuItems[i].price);
    }
}

// input the order function
void inputOrder(struct items *menuItems, int menuSize, struct order *customerOrders, int *orderCount, int orderID) {
    customerOrders[*orderCount].orderID = orderID;
    customerOrders[*orderCount].customerName = malloc(50); // capped name at 50 characters

    printf("what name should we put on the order?: ");
    fgets(customerOrders[*orderCount].customerName, 50, stdin);
    size_t len = strlen(customerOrders[*orderCount].customerName);
    if (len > 0 && customerOrders[*orderCount].customerName[len - 1] == '\n') {
        customerOrders[*orderCount].customerName[len - 1] = '\0';
    }

    int totalOrder = 0;
    int itemCount = 0;

    while (!totalOrder) {
        char input[100]; 
        printf("what would you like to order? enter '0' to finish!: ");
        fgets(input, sizeof(input), stdin);
        int menuItem = atoi(input);

        if (menuItem == 0) {
            totalOrder = 1;
        } else {
            printf("and how many would you like?: ");
            fgets(input, sizeof(input), stdin);
            int quantity = atoi(input);

            if (itemCount == 0) {
                customerOrders[*orderCount].orderItems = NULL;
            }

            customerOrders[*orderCount].orderItems = realloc(customerOrders[*orderCount].orderItems,
                                                              (itemCount + quantity) * sizeof(struct items));

      
            if (customerOrders[*orderCount].orderItems == NULL) {
                printf("memory allocation error\n");
                exit(EXIT_FAILURE);
            }

            for (int i = 0; i < quantity; i++) {
                customerOrders[*orderCount].orderItems[itemCount] = menuItems[menuItem - 1];
                itemCount++;
            }
        }
    }

    customerOrders[*orderCount].itemCount = itemCount;
    (*orderCount)++;
}

// calculate bill function
void calculateBill(struct order *customerOrders, int orderCount) {
    const float salesTaxPercentage = 0.07;

    for (int i = 0; i < orderCount; i++) {
        printf("order #%d for %s:\n", customerOrders[i].orderID, customerOrders[i].customerName);
        float totalBill = 0;

        for (int j = 0; j < customerOrders[i].itemCount; j++) {
            struct items currentItem = customerOrders[i].orderItems[j];
            printf("%d. %s - $%.2f\n", j + 1, currentItem.itemName, currentItem.price);
            totalBill += currentItem.price;
        }

        float salesTax = salesTaxPercentage * totalBill;
        float finalTotal = totalBill + salesTax;

        printf("\n---------------------\n");
        printf("subtotal: $%.2f\n", totalBill);
        printf("tax: $%.2f\n", salesTax);
        printf("total: $%.2f\n", finalTotal);
    }
}

// sales summary function
void salesSummary(struct order *customerOrders, int orderCount) {
    static int totalOrders = 0;
    static float totalRevenue = 0;

    static int itemQuantities[sizeof(menuItems) / sizeof(menuItems[0])] = {0};
    static float itemSales[sizeof(menuItems) / sizeof(menuItems[0])] = {0}; 

    totalOrders += orderCount;

    for (int i = 0; i < orderCount; i++) {
        for (int j = 0; j < customerOrders[i].itemCount; j++) {
            struct items currentItem = customerOrders[i].orderItems[j];
            totalRevenue += currentItem.price;
            for (int k = 0; k < sizeof(menuItems) / sizeof(menuItems[0]); k++) {
                if (strcmp(currentItem.itemName, menuItems[k].itemName) == 0) {
                    itemSales[k] += currentItem.price;
                    itemQuantities[k] += 1; 
                    break;
                }
            }
        }
    }

    // display sales summary
    printf("\n---------------------\n");
    printf("\ntotal sales summary:\n");
    printf("total orders: %d\n", totalOrders);
    printf("generated revenue: $%.2f\n", totalRevenue);
    printf("\n---------------------\n");
    printf("\ntotal revenue generated by each menu item:\n");
    for (int i = 0; i < sizeof(menuItems) / sizeof(menuItems[0]); i++) {
        printf("%s: $%.2f (x%d)\n", menuItems[i].itemName, itemSales[i], itemQuantities[i]);
    }

    FILE *file = fopen("salesBook.txt", "w");
    if (file == NULL) {
        fclose(file);
    }

    fprintf(file, "total sales summary:\n");
    fprintf(file, "total orders: %d\n", totalOrders);
    fprintf(file, "generated revenue: $%.2f\n", totalRevenue);
    fprintf(file, "\nitem revenue:\n");
    for (int i = 0; i < sizeof(menuItems) / sizeof(menuItems[0]); i++) {
        fprintf(file, "%s: $%.2f (x%d)\n", menuItems[i].itemName, itemSales[i], itemQuantities[i]);
    }

    fclose(file);
}
