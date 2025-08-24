#include <iostream>
#include <unordered_map>
#include <queue>
#include <string>
#include <fstream>
#include <iomanip>
#include <ctime>
using namespace std;
// ---------------- Classes ----------------
class InventoryItem {
public:
    int product_id;
    string name;
    int quantity;
    float price;
    string expiration_date; // Format: DDMMYYYY

    InventoryItem(int id, string n, int q, float p, string exp) {
        product_id = id;
        name = n;
        quantity = q;
        price = p;
        expiration_date = exp;
    }
};

class CustomerOrder {
public:
    string name;
    int product_id;
    int quantity;
    int urgency; // Priority of the order

    CustomerOrder(string n, int id, int q, int u) {
        name = n;
        product_id = id;
        quantity = q;
        urgency = u;
    }

    bool operator<(const CustomerOrder &o) const {
        return urgency < o.urgency;
    }
};

// ---------------- Globals ----------------
int next_product_id = 1;
unordered_map<int, InventoryItem*> inventory;
priority_queue<CustomerOrder> orderQueue;
const string INVENTORY_FILE = "inventory.txt";

// ---------------- Utility ----------------
bool isExpired(const string &exp) {
    if (exp.size() != 8) return true; // invalid format

    int d = stoi(exp.substr(0, 2));
    int m = stoi(exp.substr(2, 2));
    int y = stoi(exp.substr(4, 4));

    time_t t = time(0);
    tm *now = localtime(&t);

    int cd = now->tm_mday;
    int cm = now->tm_mon + 1;
    int cy = now->tm_year + 1900;

    if (y < cy) return true;
    if (y == cy && m < cm) return true;
    if (y == cy && m == cm && d < cd) return true;

    return false;
}

void saveInventory() {
    ofstream fout(INVENTORY_FILE);
    if (!fout) return;

    for (auto &p : inventory) {
        InventoryItem *item = p.second;
        fout << item->product_id << " " << item->name << " "
             << item->quantity << " " << item->price << " "
             << item->expiration_date << "\n";
    }
    fout.close();
}

void loadInventory() {
    ifstream fin(INVENTORY_FILE);
    if (!fin) return;

    int id, q;
    float price;
    string name, exp;

    while (fin >> id >> name >> q >> price >> exp) {
        inventory[id] = new InventoryItem(id, name, q, price, exp);
        next_product_id = max(next_product_id, id + 1);
    }
    fin.close();
}

// ---------------- Functions ----------------
void addItem() {
    string name, expiration_date;
    int quantity;
    float price;

    cout << "Enter item name: ";
    cin >> name;
    cout << "Enter quantity: ";
    cin >> quantity;
    if (quantity <= 0) {
        cout << "❌ Invalid quantity!\n";
        return;
    }

    cout << "Enter price: ";
    cin >> price;
    if (price <= 0) {
        cout << "❌ Invalid price!\n";
        return;
    }

    cout << "Enter expiration date (DDMMYYYY): ";
    cin >> expiration_date;
    if (expiration_date.size() != 8) {
        cout << "❌ Invalid date format!\n";
        return;
    }

    int product_id = next_product_id++;
    inventory[product_id] = new InventoryItem(product_id, name, quantity, price, expiration_date);

    cout << "Item added successfully. Product ID: " << product_id << endl;
}

void displayInventory() {
    cout << "\nCurrent Inventory:\n";
    cout << "-----------------------------------------------------------\n";
    cout << setw(5) << "ID" << setw(15) << "Name" << setw(12) << "Quantity"
         << setw(10) << "Price" << setw(15) << "Exp Date\n";
    cout << "-----------------------------------------------------------\n";

    for (const auto &pair : inventory) {
        InventoryItem *item = pair.second;
        cout << setw(5) << item->product_id
             << setw(15) << item->name
             << setw(12) << item->quantity
             << setw(10) << item->price
             << setw(15) << item->expiration_date << "\n";
    }
}

void placeOrder() {
    string customerName;
    int productId, quantity, urgency;

    cout << "Enter customer name: ";
    cin >> customerName;
    cout << "Enter product ID: ";
    cin >> productId;

    if (inventory.find(productId) == inventory.end()) {
        cout << "❌ Product not found in inventory.\n";
        return;
    }

    InventoryItem *item = inventory[productId];

    if (isExpired(item->expiration_date)) {
        cout << "❌ Product expired (" << item->expiration_date << "). Cannot place order.\n";
        return;
    }

    cout << "Enter quantity: ";
    cin >> quantity;
    if (quantity <= 0) {
        cout << "❌ Invalid quantity!\n";
        return;
    }
    if (quantity > item->quantity) {
        cout << "❌ Requested quantity exceeds available stock (" << item->quantity << ").\n";
        return;
    }

    cout << "Enter order urgency (1 - Low, 2 - Medium, 3 - High): ";
    cin >> urgency;
    if (urgency < 1 || urgency > 3) {
        cout << "❌ Invalid urgency level!\n";
        return;
    }

    orderQueue.push(CustomerOrder(customerName, productId, quantity, urgency));
    cout << "✅ Order placed successfully.\n";
}

void processOrders() {
    if (orderQueue.empty()) {
        cout << "⚠ No orders to process.\n";
        return;
    }

    CustomerOrder processedOrder = orderQueue.top();
    orderQueue.pop();
    cout << "Processing order for customer: " << processedOrder.name << endl;

    if (inventory.find(processedOrder.product_id) != inventory.end()) {
        InventoryItem *item = inventory[processedOrder.product_id];

        if (item->quantity >= processedOrder.quantity) {
            item->quantity -= processedOrder.quantity;
            cout << "✅ Order processed successfully.\n";
        } else {
            cout << "❌ Insufficient stock. Order cannot be fulfilled.\n";
        }
    } else {
        cout << "❌ Product not found in inventory.\n";
    }
}

// ---------------- Main ----------------
int main() {
    loadInventory(); // load at startup
    int choice;

    do {
        cout << "\n===========================================================\n";
        cout << "             PharmaTrack - Inventory System\n";
        cout << "===========================================================\n";
        cout << "1. Add Item\n";
        cout << "2. Display Inventory\n";
        cout << "3. Place Order\n";
        cout << "4. Process Orders\n";
        cout << "5. Exit\n";
        cout << "-----------------------------------------------------------\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: addItem(); break;
            case 2: displayInventory(); break;
            case 3: placeOrder(); break;
            case 4: processOrders(); break;
            case 5: saveInventory(); cout << "💾 Data saved. Exiting...\n"; break;
            default: cout << "❌ Invalid choice. Please try again.\n";
        }
    } while (choice != 5);

    for (auto &item : inventory) delete item.second; // free memory
    return 0;
}
