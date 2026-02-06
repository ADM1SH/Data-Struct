/*
 * LIBRARIES USED:
 * - iostream: Standard input/output stream for console interaction (cin, cout).
 * - fstream: File stream for reading from and writing to external data files (persistence).
 * - sstream: String stream for parsing and splitting strings, especially for CSV data processing.
 */
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

// ANSI Escape Codes for High-End UI Colors in the terminal
#define RED "\033[31m"
#define GRN "\033[32m"
#define YEL "\033[33m"
#define CYN "\033[36m"
#define RST "\033[0m"

// Utility Function: Removes leading and trailing whitespace from strings
// This is crucial for clean data parsing from CSV files.
string trim(string s) {
    size_t f = s.find_first_not_of(" \t\r\n"), l = s.find_last_not_of(" \t\r\n");
    return (f == string::npos) ? "" : s.substr(f, (l - f + 1));
}

// Utility Function: Converts strings to lowercase
// Used to make the search functionality case-insensitive.
string toLower(string s) {
    for (int i = 0; i < (int)s.length(); i++) {
        if (s[i] >= 'A' && s[i] <= 'Z') s[i] += 32;
    }
    return s;
}

// File Path Resolver: Checks multiple locations for data files
// Ensures the program can find the database files regardless of the execution context.
string getDataPath(string fileName) {
    string paths[] = {"data/" + fileName, "../data/" + fileName};
    for (int i = 0; i < 2; i++) {
        ifstream f(paths[i].c_str());
        if (f.is_open()) return paths[i];
    }
    return "data/" + fileName;
}

// Core Car Object: Represents a vehicle in the fleet
// Stores specifications, pricing, stock levels, and rental statistics.
struct Car {
    string id, makeModel, trans, status;
    int year, hp, ts, stock, maxStock, rentalCount;
    double rate;

    // Constructor to initialize a Car object
    Car(string i, string m, int y, int h, int t, string tr, double r, int s, int rc = 0)
        : id(i), makeModel(m), trans(tr), year(y), hp(h), ts(t), stock(s), maxStock(s), rentalCount(rc), rate(r) {
        status = (stock > 0) ? "Available" : "No Stock";
    }

    // Formatted output for displaying the car as a row in the showroom table
    void displayRow() const {
        string stockTxt = (stock > 0) ? to_string(stock) : "OUT";
        string color = (stock > 0) ? GRN : RED;

        cout << "| " << left << setw(6) << id
             << "| " << setw(35) << makeModel
             << "| " << right << setw(4) << hp << " hp "
             << "| " << left << setw(5) << trans
             << "| " << right << "$" << setw(7) << fixed << setprecision(0) << rate
             << " | " << setw(3) << stockTxt
             << " | " << color << left << setw(12) << status << RST << " |" << endl;
    }
};

// Customer Structure: Linked List node for customer records
struct Cust {
    string name, phone, carID, date;
    Cust* next;
    Cust(string n, string p, string c, string d = "") : name(n), phone(p), carID(c), date(d), next(NULL) {
        if (date == "") {
            time_t now = time(0);
            char* dt = ctime(&now);
            date = dt ? string(dt) : "Unknown Date";
            if (!date.empty() && date[date.length()-1] == '\n') date.erase(date.length()-1);
        }
    }
};

// Generic Node for Linked List of Cars
struct Node { Car* data; Node* next; Node(Car* c) : data(c), next(NULL) {} };

// QUEUE DATA STRUCTURE: Circular Array implementation for the Wash Bay
// Represents a "First-In, First-Out" (FIFO) system for servicing returned cars.
class ServiceQueue {
    Car* queue[10]; int front, rear, size;
public:
    ServiceQueue() : front(0), rear(-1), size(0) {}
    bool isFull() { return size == 10; }
    bool isEmpty() { return size == 0; }

    // Adds a car to the service queue
    void enqueue(Car* c) {
        if (isFull()) { cout << RED << "Bay Full!" << RST << endl; return; }
        rear = (rear + 1) % 10; queue[rear] = c; size++; c->status = "In-Service";
    }

    // Removes a car from the queue and updates its stock/status
    void dequeue() {
        if (isEmpty()) return;
        Car* c = queue[front]; front = (front + 1) % 10; size--;
        if (c->stock < c->maxStock) c->stock++;
        c->status = (c->stock > 0) ? "Available" : "No Stock";
    }

    // Displays all cars currently being serviced
    void display() {
        if (isEmpty()) { cout << YEL << "Wash Bay Empty" << endl << RST; return; }
        cout << CYN << "\n--- WASH BAY QUEUE ---" << RST << endl;
        for (int i = 0, j = front; i < size; i++, j = (j + 1) % 10)
            cout << i + 1 << ". " << queue[j]->makeModel << " [" << queue[j]->id << "]" << endl;
    }

    // Checks if a specific car is currently in the wash bay
    bool isInService(string id) {
        for (int i = 0, j = front; i < size; i++, j = (j + 1) % 10) if (queue[j]->id == id) return true;
        return false;
    }
};

// STACK DATA STRUCTURE: Linked List implementation for History/Logs
// Represents a "Last-In, First-Out" (LIFO) system for tracking user actions.
struct LogNode { string log; LogNode* next; LogNode(string s) : log(s), next(NULL) {} };
class HistoryStack {
    LogNode* top;
public:
    HistoryStack() : top(NULL) {}

    // Pushes a new action to the stack and logs it to a text file
    void push(string s) {
        LogNode* n = new LogNode(s); n->next = top; top = n;
        ofstream f(getDataPath("audit_log.txt").c_str(), ios::app);
        if (f.is_open()) {
            time_t now = time(0); string ts = ctime(&now);
            if (!ts.empty()) ts.erase(ts.length()-1);
            f << "[" << ts << "] " << s << endl;
        }
    }

    // Displays all logs in reverse chronological order
    void display(string title) {
        cout << CYN << "\n--- " << title << " ---" << RST << endl;
        if (!top) { cout << "Empty" << endl; return; }
        for (LogNode* t = top; t; t = t->next) cout << ">> " << t->log << endl;
    }
};

// HASH TABLE DATA STRUCTURE: For O(1) average time complexity searches
// Maps car IDs to their memory addresses for lightning-fast lookups.
class HashTable {
    Node* table[50];
public:
    HashTable() { for(int i=0; i<50; i++) table[i] = NULL; }

    // Hash Function: Converts a string ID into an array index
    int hashFn(string id) {
        int s = 0; for (int i = 0; i < (int)id.length(); i++) s += id[i];
        return s % 50;
    }

    // Insert a car into the hash table (uses chaining to handle collisions)
    void insert(Car* c) { int idx = hashFn(c->id); Node* n = new Node(c); n->next = table[idx]; table[idx] = n; }

    // Find a car by its unique ID
    Car* search(string id) {
        for (Node* t = table[hashFn(id)]; t; t = t->next) if (t->data->id == id) return t->data;
        return NULL;
    }

    // Remove a car reference from the hash table
    void remove(string id) {
        int idx = hashFn(id); Node *t = table[idx], *p = NULL;
        while (t && t->data->id != id) { p = t; t = t->next; }
        if (!t) return;
        if (!p) table[idx] = t->next; else p->next = t->next;
        delete t;
    }
};

// Showroom Management: Handles the collection of vehicles
// Uses a Linked List for storage and a Hash Table for searching.
class Showroom {
    Node* head; HashTable* ht;
public:
    Showroom(HashTable* h) : head(NULL), ht(h) {}

    // Adds a new car to the fleet
    void add(Car* c) {
        Node* n = new Node(c);
        if (!head) head = n; else { Node* t = head; while (t->next) t = t->next; t->next = n; }
        ht->insert(c);
    }

    // UI Helpers for table formatting
    void displayHeader() {
        cout << CYN << "--------------------------------------------------------------------------------------------" << endl;
        cout << "| ID    | Make & Model                       | Power   | TR   | Rate    | Qty | Status       |" << endl;
        cout << "--------------------------------------------------------------------------------------------" << RST << endl;
    }
    void displayFooter() {
        cout << CYN << "--------------------------------------------------------------------------------------------" << RST << endl;
    }

    // Displays the entire fleet
    void display() {
        displayHeader();
        for (Node* t = head; t; t = t->next) t->data->displayRow();
        displayFooter();
    }

    // Linear Search for partial brand name matches
    void searchBrand(string k) {
        bool f = false; string lk = toLower(k);
        displayHeader();
        for (Node* t = head; t; t = t->next)
            if (toLower(t->data->makeModel).find(lk) != string::npos) { t->data->displayRow(); f = true; }
        displayFooter();
        if (!f) cout << RED << "No matches found." << RST << endl;
    }

    // Bubble Sort Algorithm: Sorts by brand name alphabetically
    void sortByBrand() {
        for (Node* i = head; i; i = i->next)
            for (Node* j = i->next; j; j = j->next)
                if (i->data->makeModel > j->data->makeModel) { Car* temp = i->data; i->data = j->data; j->data = temp; }
    }

    // Selection Sort Variant: Sorts by Price or Horsepower
    void sort(int mode) { // 1: Price, 2: HP
        for (Node* i = head; i; i = i->next)
            for (Node* j = i->next; j; j = j->next)
                if ((mode == 1 && i->data->rate < j->data->rate) || (mode == 2 && i->data->hp < j->data->hp)) {
                    Car* temp = i->data; i->data = j->data; j->data = temp;
                }
    }

    // Deletes a car from the fleet if it is not currently active or rented
    void del(string id, ServiceQueue& sq) {
        Car* c = ht->search(id);
        if (!c || c->status == "In-Service" || sq.isInService(id) || c->stock < c->maxStock) {
            cout << RED << "Error: Car is active or not found." << endl; return;
        }
        Node *t = head, *p = NULL;
        while (t && t->data->id != id) { p = t; t = t->next; }
        if (!p) head = t->next; else p->next = t->next;
        ht->remove(id); delete t->data; delete t; cout << YEL << "Decommissioned." << RST << endl;
    }

    Car* get(string id) { return ht->search(id); }

    // Business Logic: Calculates total asset value and revenue
    void showAnalytics(double sessionRevenue) {
        double val = 0; Car* pop = NULL;
        for (Node* t = head; t; t = t->next) {
            val += (t->data->rate * t->data->maxStock);
            if (!pop || t->data->rentalCount > pop->rentalCount) pop = t->data;
        }
        cout << CYN << "\n--- THE PADDOCK ANALYTICS ---\n" << RST
             << "Total Asset Value:    $" << fixed << setprecision(2) << val << endl;
        cout << "Session Revenue:      $" << sessionRevenue << endl;
        cout << "Most Popular Vehicle: " << (pop && pop->rentalCount > 0 ? pop->makeModel : "N/A") << endl;
    }

    // Data Persistence: Saves current fleet state to CSV
    void saveToFile() {
        ofstream f(getDataPath("db_fleet.csv").c_str());
        for (Node* t = head; t; t = t->next)
            f << t->data->id << "," << t->data->makeModel << "," << t->data->year << "," << t->data->hp << "," << t->data->ts
              << "," << t->data->trans << "," << t->data->rate << "," << t->data->stock << "," << t->data->maxStock << "," << t->data->rentalCount << endl;
    }

    // Data Persistence: Loads fleet data from CSV
    void loadFromFile() {
        string path = getDataPath("db_fleet.csv"); ifstream f(path.c_str());
        if (!f.is_open()) return;
        string line;
        while (getline(f, line)) {
            if (line.length() < 5) continue;
            vector<string> col; string part; stringstream ss(line);
            while (getline(ss, part, ',')) col.push_back(trim(part));
            if (col.size() >= 7) {
                try {
                    Car* c = new Car(col[0], col[1], stoi(col[2]), stoi(col[3]), stoi(col[4]), col[5], stod(col[6]),
                                     col.size() > 7 ? stoi(col[7]) : 1, col.size() > 9 ? stoi(col[9]) : 0);
                    c->maxStock = col.size() > 8 ? stoi(col[8]) : c->stock;
                    add(c);
                } catch(...) {}
            }
        }
        sortByBrand();
    }
};

// Global Helpers: Displays the ASCII Art Header
void header() {
    cout << CYN << "  _______ _    _  ______   _____             _____   _____    ____    _____  _  __\n"
         << " |__   __| |  | | |  ____| |  __ \\    /\\    |  __ \\ |  __ \\  / __ \\  / ____|| |/ /\n"
         << "    | |  | |__| | | |__    | |__) |  /  \\   | |  | || |  | || |  | || |     | ' / \n"
         << "    | |  |  __  | |  __|   |  ___/  / /\\ \\  | |  | || |  | || |  | || |     |  <  \n"
         << "    | |  | |  | | | |____  | |     / ____ \\ | |__| || |__| || |__| || |____ | . \\ \n"
         << "    |_|  |_|  |_| |______| |_|    /_/    \\_\\|_____/ |_____/  \\____/  \\_____||_|\\_\\\n"
         << "\n                       _____  _       _    _  ____  \n"
         << "                      / ____|| |     | |  | ||  _ \\ \n"
         << "                     | |     | |     | |  | || |_) |\n"
         << "                     | |     | |     | |  | ||  _ < \n"
         << "                     | |____ | |____ | |__| || |_) |\n"
         << "                      \\_____||______| \\____/ |____/ \n" << RST
         << "\n                      EXOTIC & PERFORMANCE CAR RENTALS\n==================================================================================\n" << endl;
}

// Global Function: Updates the revenue report text file
void updateRevenue(double rev) {
    ofstream f(getDataPath("Revenue_Report.txt").c_str());
    if (f.is_open()) { time_t n = time(0); f << "PADDOCK CLUB FINANCIAL REPORT\nGenerated: " << ctime(&n) << "Total Revenue: $" << fixed << setprecision(2) << rev << endl; }
}

// Global Function: Saves customer linked list to CSV
void saveCust(Cust* h, Showroom& sr) {
    ofstream f(getDataPath("db_customers.csv").c_str());
    for (Cust* t = h; t; t = t->next) {
        f << t->name << "," << t->phone << "," << t->carID << "," << t->date << endl;
    }
}

// Global Function: Loads customer data from CSV into a Linked List
Cust* loadCust() {
    ifstream f(getDataPath("db_customers.csv").c_str());
    string l; Cust* h = NULL;
    while (getline(f, l)) {
        stringstream ss(l); string n, p, id, d;
        if (getline(ss, n, ',') && getline(ss, p, ',') && getline(ss, id, ',') && getline(ss, d)) {
            Cust* nc = new Cust(n, p, id, d); nc->next = h; h = nc; 
        }
    }
    return h;
}

// Global Function: Retrieves historical revenue data from the report file
double loadRevenue() {
    ifstream f(getDataPath("Revenue_Report.txt").c_str());
    string line; double rev = 0;
    while(getline(f, line)){
        if(line.find("Total Revenue: $") != string::npos){
            try { rev = stod(line.substr(line.find("$") + 1)); } catch(...) { rev = 0; }
        }
    }
    return rev;
}

// Clears input buffer to prevent infinite loops on invalid input
void clear() { cin.clear(); cin.ignore(1000, '\n'); }

// Business Logic: Processes a car rental transaction
// Includes age verification and high-performance car restrictions.
bool handleRental(Showroom& sr, Cust** h, HistoryStack& rs, double& rev, string id) {
    int age, days; Car* c = sr.get(id);
    if (!c || c->stock <= 0) { cout << RED << "Unavailable!" << RST << endl; return false; }
    cout << "Age: "; if (!(cin >> age) || age < 18) { cout << RED << "Denied: Underage!" << RST << endl; clear(); return false; }
    if (c->hp > 500 && age < 25) { cout << RED << "Power Restricted (25+ required)!" << RST << endl; return false; }
    string name, phone; cout << "Name: "; cin.ignore(); getline(cin, name);
    cout << "Phone: "; cin >> phone; cout << "Days: "; if (!(cin >> days) || days <= 0) { cout << RED << "Invalid days!" << endl; clear(); return false; }
    c->stock--; c->rentalCount++; rev += (c->rate * days);
    if (c->stock == 0) c->status = "Rented (Out)";
    Cust* nc = new Cust(name, phone, id); nc->next = *h; *h = nc;
    rs.push("Rented " + c->makeModel + " to " + name);
    sr.saveToFile(); saveCust(*h, sr); updateRevenue(rev);
    cout << GRN << "\n--- BOOKING CONFIRMED ---\nTotal: $" << fixed << setprecision(2) << c->rate * days << RST << endl;
    return true;
}

// Main Execution Loop: Provides the interactive menu for the user
int main() {
    // Initializing Data Structures
    HashTable ht; Showroom sr(&ht); ServiceQueue sq; HistoryStack rs, ss;
    Cust* custHead = loadCust(); string pass = "paddock77", pInput, id;
    double sessionRev = loadRevenue();

    // Load admin password from file
    ifstream pf(getDataPath("admin_pass.txt").c_str()); if (pf.is_open()) { getline(pf, pass); pf.close(); }

    // Load fleet from database
    sr.loadFromFile();

    int c;
    while (true) {
        system("clear"); header();
        cout << CYN << "[ MAIN NAVIGATION ]" << RST << endl;
        cout << left << setw(25) << "1. View Showroom" << setw(25) << "2. Search Catalog" << endl;
        cout << left << setw(25) << "3. Sort Catalog" << setw(25) << "4. Rent Vehicle" << endl;
        cout << left << setw(25) << "5. Return Vehicle" << setw(25) << "6. Wash Bay Queue" << endl;
        cout << left << setw(25) << "7. Activity Logs" << setw(25) << "8. Admin Console" << endl;
        cout << left << setw(25) << "0. Exit System" << endl;
        cout << CYN << "\nChoice: " << RST;

        if (!(cin >> c)) { clear(); continue; }
        if (c == 0) break;

        // Navigation Logic based on user input
        if (c == 1) {
            sr.display();
            char b; cout << "\nBook? (y/n): "; cin >> b;
            if (tolower(b) == 'y') { cout << "ID: "; cin >> id; handleRental(sr, &custHead, rs, sessionRev, id); }
        }
        else if (c == 2) {
            string k; cout << "Keyword: "; cin >> k;
            ss.push("Searched: " + k); sr.searchBrand(k);
        }
        else if (c == 3) {
            int s; cout << "[1] Price [2] Power [3] Brand: "; cin >> s;
            if (s == 3) sr.sortByBrand(); else sr.sort(s);
            sr.display();
        }
        else if (c == 4) {
            cout << "ID: "; cin >> id; handleRental(sr, &custHead, rs, sessionRev, id);
        }
        else if (c == 5) {
            cout << "ID: "; cin >> id;
            Car* cr = sr.get(id);
            if (cr && cr->stock < cr->maxStock && cr->status != "In-Service") {
                sq.enqueue(cr); rs.push("Returned " + cr->makeModel);
            } else cout << RED << "Error!" << endl;
        }
        else if (c == 6) {
            sq.display();
            if (!sq.isEmpty()) { char y; cout << "Finish? (y/n): "; cin >> y; if (tolower(y) == 'y') sq.dequeue(); }
        }
        else if (c == 7) {
            rs.display("RENTAL ACTIVITY LOGS");
            ss.display("SEARCH HISTORY LOGS");
        }
        else if (c == 8) {
            cout << "Pass: "; cin >> pInput;
            if (pInput == pass) {
                int a; cout << CYN << "\n--- ADMIN ---" << RST << "\n1.Add 2.Del 3.Cust 4.Stats 5.Pass: "; cin >> a;
                if (a == 1) {
                    string i, m, t; int y, h, ts, s; double r;
                    cout << "ID/Model/Y/HP/TS/Rate/Stock: "; cin >> i; cin.ignore(); getline(cin, m);
                    cin >> y >> h >> ts >> r >> s; cout << "TR: "; cin >> t;
                    sr.add(new Car(i, m, y, h, ts, t, r, s)); sr.saveToFile();
                }
                if (a == 2) { cout << "ID: "; cin >> id; sr.del(id, sq); sr.saveToFile(); }
                if (a == 3) { for (Cust* tC = custHead; tC; tC = tC->next) cout << tC->name << " | " << tC->phone << " | " << tC->carID << endl; }
                if (a == 4) sr.showAnalytics(sessionRev);
                if (a == 5) { cin >> pass; ofstream f(getDataPath("admin_pass.txt").c_str()); f << pass; }
            }
        }
        cout << "\n(Enter to continue...)"; cin.ignore(); cin.get();
    }
    return 0;
}