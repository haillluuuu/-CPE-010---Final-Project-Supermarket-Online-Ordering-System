#ifndef SUPERMARKET_H
#define SUPERMARKET_H

#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <ctime>

// Structure representing a product in the supermarket inventory
struct Product {
    int id = 0;
    std::string name;
    std::string category;
    double price = 0.0;
    int stock = 0;
};

// Structure representing one line item inside a shopping cart
struct CartItem {
    int productId = 0;
    std::string name;
    double price = 0.0;
    int qty = 0;

    // Compares items by product ID (used by DLL search/deletion)
    bool operator==(const CartItem& other) const {
        return productId == other.productId;
    }
};

// Stream insertion operator to print a CartItem
inline std::ostream& operator<<(std::ostream& os, const CartItem& item) {
    os << item.name << " x" << item.qty;
    return os;
}

// Structure representing a customer order placed in the system
struct Order {
    int orderId = 0;
    std::string customerName;
    std::string contact;
    std::string address;
    std::vector<CartItem> items; // Snapshot of items at checkout
    double total = 0.0;
    double paid = 0.0;
    double change = 0.0;
    std::string status;          // Preparing / Out for Delivery / Delivered / Cancelled
    std::string dateTime;

    // Compares orders by order ID
    bool operator==(const Order& other) const {
        return orderId == other.orderId;
    }
};

// Structure representing the current customer profile
struct Customer {
    std::string name;
    std::string contact;
    std::string address;
    bool loggedIn = false;
};

// Converts a string to lowercase for case-insensitive operations
inline std::string toLower(std::string s) {
    for (std::size_t i = 0; i < s.size(); i++)
        s[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(s[i])));
    return s;
}

// Generates the current timestamp in YYYY-MM-DD HH:MM format
inline std::string currentDateTime() {
    std::time_t now = std::time(nullptr);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", std::localtime(&now));
    return std::string(buf);
}

// Custom comparator helpers for sorting operations
inline bool lessById(const Product& a, const Product& b) {
    return a.id < b.id;
}

inline bool lessByName(const Product& a, const Product& b) {
    std::string x = toLower(a.name), y = toLower(b.name);
    if (x != y) return x < y;
    return a.id < b.id;
}

inline bool lessByCategory(const Product& a, const Product& b) {
    std::string x = toLower(a.category), y = toLower(b.category);
    if (x != y) return x < y;
    return lessByName(a, b);
}

inline bool lessByPriceAsc(const Product& a, const Product& b) {
    if (a.price != b.price) return a.price < b.price;
    return a.id < b.id;
}

inline bool lessByPriceDesc(const Product& a, const Product& b) {
    if (a.price != b.price) return a.price > b.price;
    return a.id < b.id;
}

#endif