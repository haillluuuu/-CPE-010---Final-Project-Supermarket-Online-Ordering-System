//main.cpp
// Supermarket Online Ordering System
// Group 3

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>

#include "Supermarket.h"
#include "DoublyLinkedList.h"
#include "Stack.h"
#include "Queue.h"
#include "SearchingAlgo.h"
#include "SortingAlgo.h"

// Global System Variables
std::vector<Product> inventory;
DoubleList<CartItem>* cartHead = nullptr;
Stack<CartItem> cartActionStack; // Stack for Undo feature (LIFO)
Queue<Order> orderQueue;         // FIFO Queue for processing orders
DoubleList<Order>* orderHistoryHead = nullptr;
Customer currentCustomer;
int nextOrderId = 1001;

// Clears input stream state and flushes input buffer
void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Prints a divider line using a specified character
void printSeparator(char ch = '=', int length = 60) {
    std::cout << std::string(length, ch) << "\n";
}

// Displays formatted header title
void printHeader(const std::string& title) {
    printSeparator('=');
    std::cout << "  " << title << "\n";
    printSeparator('=');
}

// Seeds default product inventory records
void seedInitialProducts() {
    inventory = {
        {101, "Fresh Milk 1L", "Dairy", 95.50, 50},
        {102, "Cheddar Cheese 200g", "Dairy", 140.00, 30},
        {103, "Whole Wheat Bread", "Bakery", 65.00, 40},
        {104, "Chocolate Chip Cookies", "Bakery", 85.00, 25},
        {105, "Red Apples (1kg)", "Produce", 120.00, 60},
        {106, "Organic Bananas (1kg)", "Produce", 75.00, 45},
        {107, "Mineral Water 1.5L", "Beverages", 35.00, 100},
        {108, "Orange Juice 1L", "Beverages", 110.00, 20},
        {109, "Potato Chips 150g", "Snacks", 55.00, 80},
        {110, "Dark Chocolate 100g", "Snacks", 99.00, 35}
    };
}

// Sorts inventory vector using Quick Sort algorithm
template <typename Comparator>
void sortInventory(Comparator comp) {
    if (!inventory.empty()) {
        quickSort(inventory, 0, static_cast<int>(inventory.size()) - 1, comp);
    }
}

// Searches product by ID using Binary Search algorithm
int searchProductById(int id) {
    sortInventory(lessById);
    return binarySearchVector(inventory, id, [](const Product& p) { return p.id; });
}

// Searches product by Name using Binary Search algorithm
int searchProductByName(const std::string& name) {
    sortInventory(lessByName);
    std::string target = toLower(name);
    return binarySearchVector(inventory, target, [](const Product& p) { return toLower(p.name); });
}

// Prints tabular view of product inventory list
void displayProductTable(const std::vector<Product>& products) {
    if (products.empty()) {
        std::cout << "No products available.\n";
        return;
    }

    std::cout << std::left 
              << std::setw(8)  << "ID" 
              << std::setw(26) << "Name" 
              << std::setw(15) << "Category" 
              << std::setw(12) << "Price (PHP)" 
              << std::setw(8)  << "Stock" << "\n";
    printSeparator('-', 69);

    for (const auto& prod : products) {
        std::cout << std::left 
                  << std::setw(8)  << prod.id 
                  << std::setw(26) << prod.name 
                  << std::setw(15) << prod.category 
                  << std::setw(12) << std::fixed << std::setprecision(2) << prod.price 
                  << std::setw(8)  << prod.stock << "\n";
    }
}

// Displays customer shopping cart items (DLL traversal)
void displayCart() {
    printHeader("YOUR SHOPPING CART");
    if (cartHead == nullptr) {
        std::cout << "Your cart is empty.\n\n";
        return;
    }

    DoubleList<CartItem>* curr = cartHead;
    double total = 0.0;
    int index = 1;

    std::cout << std::left 
              << std::setw(6)  << "Item" 
              << std::setw(8)  << "ID" 
              << std::setw(25) << "Name" 
              << std::setw(10) << "Price" 
              << std::setw(6)  << "Qty" 
              << std::setw(10) << "Subtotal" << "\n";
    printSeparator('-', 65);

    while (curr != nullptr) {
        double subtotal = curr->data.price * curr->data.qty;
        total += subtotal;
        std::cout << std::left 
                  << std::setw(6)  << index++ 
                  << std::setw(8)  << curr->data.productId 
                  << std::setw(25) << curr->data.name 
                  << std::setw(10) << std::fixed << std::setprecision(2) << curr->data.price 
                  << std::setw(6)  << curr->data.qty 
                  << std::setw(10) << subtotal << "\n";
        curr = curr->next;
    }
    printSeparator('-', 65);
    std::cout << "Grand Total: PHP " << std::fixed << std::setprecision(2) << total << "\n\n";
}

// Prints summary details for an individual order
void displaySingleOrderDetails(const Order& order) {
    std::cout << "Order ID     : #" << order.orderId << "\n";
    std::cout << "Customer     : " << order.customerName << " (" << order.contact << ")\n";
    std::cout << "Address      : " << order.address << "\n";
    std::cout << "Date/Time    : " << order.dateTime << "\n";
    std::cout << "Status       : " << order.status << "\n";
    std::cout << "Items        :\n";
    for (const auto& item : order.items) {
        std::cout << "  - " << item.name << " x" << item.qty 
                  << " @ PHP " << std::fixed << std::setprecision(2) << item.price 
                  << " = PHP " << item.price * item.qty << "\n";
    }
    std::cout << "Total Amount : PHP " << std::fixed << std::setprecision(2) << order.total << "\n";
    std::cout << "Paid (E-Cash): PHP " << order.paid << "\n";
    std::cout << "Change       : PHP " << order.change << "\n";
}

// mandatory customer login screen before main menu access
bool customerLogin() {
    printHeader("CUSTOMER LOGIN / PROFILE SETUP");
    std::string name, contact, address;

    std::cout << "Enter your Full Name: ";
    std::getline(std::cin >> std::ws, name);
    std::cout << "Enter Contact Number: ";
    std::getline(std::cin, contact);
    std::cout << "Enter Delivery Address: ";
    std::getline(std::cin, address);

    if (name.empty() || contact.empty() || address.empty()) {
        std::cout << "\nError: All profile fields are required to log in!\n";
        return false;
    }

    currentCustomer.name = name;
    currentCustomer.contact = contact;
    currentCustomer.address = address;
    currentCustomer.loggedIn = true;

    std::cout << "\nLogin Successful! Welcome, " << currentCustomer.name << ".\n\n";
    return true;
}

// mandatory admin and staff password authentication
bool adminLogin() {
    printHeader("STAFF / ADMIN AUTHENTICATION");
    std::string password;
    std::cout << "Enter Staff Access Code: ";
    std::cin >> password;

    // Accepts 'admin123' or '1234'
    if (password == "admin123" || password == "1234") {
        std::cout << "\nAccess Granted. Welcome, Staff/Admin!\n\n";
        return true;
    } else {
        std::cout << "\nAccess Denied: Incorrect Passcode.\n\n";
        return false;
    }
}

// Interactive catalog browsing, sorting, and binary searching
void browseProductsMenu() {
    int choice = 0;
    do {
        printHeader("BROWSE & SEARCH PRODUCTS");
        std::cout << "1. View All Products (Default)\n";
        std::cout << "2. Sort Products by Name (Quick Sort A-Z)\n";
        std::cout << "3. Sort Products by Category (Quick Sort)\n";
        std::cout << "4. Sort Products by Price (Quick Sort Ascending)\n";
        std::cout << "5. Sort Products by Price (Quick Sort Descending)\n";
        std::cout << "6. Search Product by ID (Binary Search)\n";
        std::cout << "7. Search Product by Name (Binary Search)\n";
        std::cout << "8. Back to Customer Menu\n";
        std::cout << "Choice: ";

        if (!(std::cin >> choice)) {
            clearInputBuffer();
            continue;
        }

        switch (choice) {
            case 1:
                sortInventory(lessById);
                std::cout << "\n";
                displayProductTable(inventory);
                break;
            case 2:
                sortInventory(lessByName);
                std::cout << "Sorted by Name (A-Z)";
                displayProductTable(inventory);
                break;
            case 3:
                sortInventory(lessByCategory);
                std::cout << "Sorted by Category";
                displayProductTable(inventory);
                break;
            case 4:
                sortInventory(lessByPriceAsc);
                std::cout << "Sorted by Price (Ascending)";
                displayProductTable(inventory);
                break;
            case 5:
                sortInventory(lessByPriceDesc);
                std::cout << "Sorted by Price (Descending)";
                displayProductTable(inventory);
                break;
            case 6: {
                int searchId = 0;
                std::cout << "Enter Product ID to search: ";
                std::cin >> searchId;
                int idx = searchProductById(searchId);
                if (idx != -1) {
                    std::cout << "\nProduct Found:\n";
                    displayProductTable({inventory[idx]});
                } else {
                    std::cout << "Product with ID " << searchId << " not found.\n";
                }
                break;
            }
            case 7: {
                std::string searchName;
                std::cout << "Enter Product Name to search: ";
                clearInputBuffer();
                std::getline(std::cin, searchName);
                int idx = searchProductByName(searchName);
                if (idx != -1) {
                    std::cout << "\nProduct Found:\n";
                    displayProductTable({inventory[idx]});
                } else {
                    std::cout << "Product \"" << searchName << "\" not found.\n";
                }
                break;
            }
            case 8:
                return;
            default:
                std::cout << "Invalid choice.\n";
        }
        std::cout << "\n";
    } while (choice != 8);
}

// Adds selected product to cart DLL and pushes action to Undo Stack
void addToCart() {
    printHeader("ADD ITEM TO CART");
    sortInventory(lessById);
    displayProductTable(inventory);

    int productId = 0, qty = 0;
    std::cout << "\nEnter Product ID to add: ";
    if (!(std::cin >> productId)) {
        clearInputBuffer();
        return;
    }

    int idx = searchProductById(productId);
    if (idx == -1) {
        std::cout << "Error: Invalid Product ID.\n";
        return;
    }

    Product& prod = inventory[idx];
    std::cout << "Selected: " << prod.name << " (Stock available: " << prod.stock << ")\n";
    std::cout << "Enter Quantity: ";
    std::cin >> qty;

    if (qty <= 0) {
        std::cout << "Quantity must be greater than zero.\n";
        return;
    }

    if (qty > prod.stock) {
        std::cout << "Error: Insufficient stock available.\n";
        return;
    }

    CartItem tempSearch;
    tempSearch.productId = prod.id;
    DoubleList<CartItem>* existing = DllFind(tempSearch, cartHead);

    if (existing != nullptr) {
        if (existing->data.qty + qty > prod.stock) {
            std::cout << "Error: Total cart quantity exceeds available stock.\n";
            return;
        }
        existing->data.qty += qty;
    } else {
        CartItem newItem{prod.id, prod.name, prod.price, qty};
        DllInsertEnd(newItem, cartHead);
    }

    // Push addition record onto action Stack for Undo operations
    cartActionStack.push({prod.id, prod.name, prod.price, qty});

    std::cout << "Success: Added " << qty << " x " << prod.name << " to cart.\n";
}

// Removes a selected item node from cart DLL
void removeFromCart() {
    if (cartHead == nullptr) {
        std::cout << "Your cart is already empty.\n";
        return;
    }

    displayCart();
    int productId = 0;
    std::cout << "Enter the product ID to remove from cart: ";
    std::cin >> productId;

    CartItem tempSearch;
    tempSearch.productId = productId;

    DoubleList<CartItem>* node = DllFind(tempSearch, cartHead);
    if (node == nullptr) {
        std::cout << "Item not found in cart.\n";
        return;
    }

    DllDelete(tempSearch, cartHead);
    std::cout << "Item removed successfully from cart.\n";
}

// Pops recent addition from action Stack and reverses its effect on cart DLL
void undoLastCartAdd() {
    printHeader("UNDO LAST CART ADDITION");
    if (cartActionStack.isEmpty()) {
        std::cout << "No recently added item  in cart to undo.\n";
        return;
    }

    CartItem lastAction = cartActionStack.pop();
    CartItem tempSearch;
    tempSearch.productId = lastAction.productId;

    DoubleList<CartItem>* node = DllFind(tempSearch, cartHead);
    if (node != nullptr) {
        if (node->data.qty <= lastAction.qty) {
            DllDelete(tempSearch, cartHead);
        } else {
            node->data.qty -= lastAction.qty;
        }
        std::cout << "Undo Successful: Removed " << lastAction.qty << " x " << lastAction.name << " from cart.\n";
    } else {
        std::cout << "Could not locate item to undo.\n";
    }
}

// Processes order payment, reduces inventory stock, enqueues order into FIFO Queue
void checkout() {
    if (cartHead == nullptr) {
        std::cout << "Your cart is empty! Cannot proceed to checkout.\n";
        return;
    }

    displayCart();

    double total = 0.0;
    std::vector<CartItem> itemSnapshot;
    DoubleList<CartItem>* curr = cartHead;

    while (curr != nullptr) {
        int idx = searchProductById(curr->data.productId);
        if (idx == -1 || inventory[idx].stock < curr->data.qty) {
            std::cout << "Error: Stock level for item '" << curr->data.name 
                      << "' has changed. Please update your cart.\n";
            return;
        }
        itemSnapshot.push_back(curr->data);
        total += (curr->data.price * curr->data.qty);
        curr = curr->next;
    }

    std::cout << "Checkout Total: PHP " << std::fixed << std::setprecision(2) << total << "\n";
    std::cout << "Select Payment Method:\n1. E-Cash (GCash / Maya)\n2. Cancel Checkout\nChoice: ";
    int payChoice = 0;
    std::cin >> payChoice;

    if (payChoice != 1) {
        std::cout << "Checkout cancelled.\n";
        return;
    }

    double cashPaid = 0.0;
    std::cout << "Enter E-Cash Amount: PHP ";
    std::cin >> cashPaid;

    if (cashPaid < total) {
        std::cout << "Error: Insufficient payment amount. Checkout cancelled.\n";
        return;
    }

    // Deduct stock levels from inventory
    for (const auto& item : itemSnapshot) {
        int idx = searchProductById(item.productId);
        if (idx != -1) {
            inventory[idx].stock -= item.qty;
        }
    }

    // Create Order object
    Order newOrder;
    newOrder.orderId = nextOrderId++;
    newOrder.customerName = currentCustomer.name;
    newOrder.contact = currentCustomer.contact;
    newOrder.address = currentCustomer.address;
    newOrder.items = itemSnapshot;
    newOrder.total = total;
    newOrder.paid = cashPaid;
    newOrder.change = cashPaid - total;
    newOrder.status = "Preparing";
    newOrder.dateTime = currentDateTime();

    // Enqueue order into Queue
    enqueue(orderQueue, newOrder);

    // Record order entry in master history DLL
    DllInsertEnd(newOrder, orderHistoryHead);

    // Empty cart DLL and clear action Stack
    DllDeleteList(cartHead);
    cartActionStack.clear();

    printHeader("ORDER PLACED SUCCESSFULLY");
    displaySingleOrderDetails(newOrder);
    std::cout << "\nYour order has been queued for fulfillment!\n";
}

// Cancels pending order in queue and returns product stocks to inventory
void cancelOrder() {
    if (queueIsEmpty(orderQueue)) {
        std::cout << "No pending orders to cancel.\n";
        return;
    }

    int cancelId = 0;
    std::cout << "Enter Order ID to cancel: ";
    std::cin >> cancelId;

    Order dummyOrder;
    dummyOrder.orderId = cancelId;

    DoubleList<Order>* orderNode = queueFind(orderQueue, dummyOrder);
    if (orderNode == nullptr) {
        std::cout << "Order ID #" << cancelId << " was not found in the active processing queue.\n";
        return;
    }

    Order& targetOrder = orderNode->data;

    if (targetOrder.status != "Preparing") {
        std::cout << "Order cannot be cancelled because it is already '" << targetOrder.status << "'.\n";
        return;
    }

    // Return item quantities back to inventory stock
    for (const auto& item : targetOrder.items) {
        int idx = searchProductById(item.productId);
        if (idx != -1) {
            inventory[idx].stock += item.qty;
        }
    }

    // Update status in master history DLL
    DoubleList<Order>* histNode = DllFind(dummyOrder, orderHistoryHead);
    if (histNode != nullptr) {
        histNode->data.status = "Cancelled";
    }

    // Unlink and delete node from order queue
    removeFromQueue(orderQueue, dummyOrder);
    std::cout << "Order #" << cancelId << " has been successfully cancelled and stock returned to inventory.\n";
}

// Customer main menu loop
void customerMenu() {
    int choice = 0;
    do {
        printHeader("CUSTOMER MAIN MENU");
        std::cout << "Logged in as: " << currentCustomer.name << " (" << currentCustomer.contact << ")\n";
        printSeparator('-', 30);
        std::cout << "1. Browse & Search Products\n";
        std::cout << "2. Add Item to Cart\n";
        std::cout << "3. View Shopping Cart\n";
        std::cout << "4. Remove Item from Cart\n";
        std::cout << "5. Undo Last Cart Addition (Stack LIFO)\n";
        std::cout << "6. Checkout & Pay\n";
        std::cout << "7. Cancel Pending Order\n";
        std::cout << "8. Logout & Return to Portal Selection\n";
        std::cout << "Choice: ";

        if (!(std::cin >> choice)) {
            clearInputBuffer();
            continue;
        }

        switch (choice) {
            case 1: browseProductsMenu(); break;
            case 2: addToCart(); break;
            case 3: displayCart(); break;
            case 4: removeFromCart(); break;
            case 5: undoLastCartAdd(); break;
            case 6: checkout(); break;
            case 7: cancelOrder(); break;
            case 8: 
                currentCustomer.loggedIn = false;
                std::cout << "Logged out successfully.\n";
                break;
            default: std::cout << "Invalid selection.\n";
        }
    } while (choice != 8);
}

// Admin menu
// prints all pending orders inside FIFO queue
void viewOrderQueue() {
    printHeader("ACTIVE ORDER QUEUE (FIFO)");
    if (queueIsEmpty(orderQueue)) {
        std::cout << "No orders currently pending in queue.\n";
        return;
    }

    DoubleList<Order>* curr = orderQueue.front;
    int pos = 1;
    while (curr != nullptr) {
        std::cout << "--- Queue Position #" << pos++ << " ---\n";
        displaySingleOrderDetails(curr->data);
        printSeparator('-', 40);
        curr = curr->next;
    }
}

// Admin menu
// dequeues front order and updates status to Out for Delivery
void processNextOrder() {
    printHeader("PROCESS NEXT ORDER");
    if (queueIsEmpty(orderQueue)) {
        std::cout << "Queue is empty. No orders to process.\n";
        return;
    }

    Order processedOrder;
    dequeue(orderQueue, processedOrder);

    DoubleList<Order>* histNode = DllFind(processedOrder, orderHistoryHead);
    if (histNode != nullptr) {
        histNode->data.status = "Out for Delivery";
    }

    std::cout << "Order #" << processedOrder.orderId << " for " 
              << processedOrder.customerName << " processed and set to 'Out for Delivery'!\n";
}

// Admin menu
// managing product creations and restocking
void manageInventoryMenu() {
    int choice = 0;
    do {
        printHeader("INVENTORY MANAGEMENT");
        std::cout << "1. Add New Product\n";
        std::cout << "2. Restock Existing Product\n";
        std::cout << "3. View All Products\n";
        std::cout << "4. Back to Admin Menu\n";
        std::cout << "Choice: ";

        if (!(std::cin >> choice)) {
            clearInputBuffer();
            continue;
        }

        switch (choice) {
            case 1: {
                Product p;
                std::cout << "Enter New Product ID: ";
                std::cin >> p.id;
                if (searchProductById(p.id) != -1) {
                    std::cout << "Error: Product ID already exists.\n";
                    break;
                }
                clearInputBuffer();
                std::cout << "Enter Product Name: ";
                std::getline(std::cin, p.name);
                std::cout << "Enter Category: ";
                std::getline(std::cin, p.category);
                std::cout << "Enter Price: ";
                std::cin >> p.price;
                std::cout << "Enter Initial Stock: ";
                std::cin >> p.stock;

                inventory.push_back(p);
                std::cout << "Product successfully added!\n";
                break;
            }
            case 2: {
                int id = 0, addStock = 0;
                std::cout << "Enter Product ID to Restock: ";
                std::cin >> id;
                int idx = searchProductById(id);
                if (idx == -1) {
                    std::cout << "Product ID not found.\n";
                    break;
                }
                std::cout << "Current Stock for " << inventory[idx].name << " is " << inventory[idx].stock << "\n";
                std::cout << "Enter Stock Quantity to Add: ";
                std::cin >> addStock;
                if (addStock > 0) {
                    inventory[idx].stock += addStock;
                    std::cout << "Updated Stock: " << inventory[idx].stock << "\n";
                }
                break;
            }
            case 3:
                sortInventory(lessById);
                displayProductTable(inventory);
                break;
            case 4:
                return;
            default:
                std::cout << "Invalid choice.\n";
        }
    } while (choice != 4);
}

// Admin menu
// views complete order history log from master DLL
void viewAllOrderHistory() {
    printHeader("ALL HISTORICAL ORDERS");
    if (orderHistoryHead == nullptr) {
        std::cout << "No orders have been recorded yet.\n";
        return;
    }

    DoubleList<Order>* curr = orderHistoryHead;
    while (curr != nullptr) {
        displaySingleOrderDetails(curr->data);
        printSeparator('-', 50);
        curr = curr->next;
    }
}

// Administrative menu loop
void adminMenu() {
    int choice = 0;
    do {
        printHeader("SUPERMARKET ADMIN MENU");
        std::cout << "1. View Active Order Queue (FIFO)\n";
        std::cout << "2. Process Next Order (Dequeue)\n";
        std::cout << "3. Inventory Management (Add/Restock)\n";
        std::cout << "4. View Master Order History\n";
        std::cout << "5. Return to Portal Selection\n";
        std::cout << "Choice: ";

        if (!(std::cin >> choice)) {
            clearInputBuffer();
            continue;
        }

        switch (choice) {
            case 1: viewOrderQueue(); break;
            case 2: processNextOrder(); break;
            case 3: manageInventoryMenu(); break;
            case 4: viewAllOrderHistory(); break;
            case 5: std::cout << "Returning to portal selection...\n"; break;
            default: std::cout << "Invalid choice.\n";
        }
    } while (choice != 5);
}

// Cleans up dynamic allocations upon program termination
void cleanupSystem() {
    DllDeleteList(cartHead);
    clearQueue(orderQueue);
    DllDeleteList(orderHistoryHead);
}

// Main execution routine
int main() {
    seedInitialProducts();

    int roleChoice = 0;
    do {
        printHeader("ONLINE SUPERMARKET MANAGEMENT SYSTEM");
        std::cout << "Select Access Portal:\n";
        std::cout << "1. Customer Portal (Login Required)\n";
        std::cout << "2. Admin / Supermarket Operations (Passcode Required)\n";
        std::cout << "3. Exit System\n";
        std::cout << "Choice: ";

        if (!(std::cin >> roleChoice)) {
            clearInputBuffer();
            std::cout << "Invalid input. Please enter a number.\n\n";
            continue;
        }

        switch (roleChoice) {
            case 1:
                if (customerLogin()) {
                    customerMenu();
                }
                break;
            case 2:
                if (adminLogin()) {
                    adminMenu();
                }
                break;
            case 3:
                std::cout << "\nThank you for using Supermarket Management System. Goodbye!\n";
                break;
            default:
                std::cout << "Invalid selection. Please choose 1, 2, or 3.\n";
        }
        std::cout << "\n";
    } while (roleChoice != 3);

    cleanupSystem();
    return 0;
}