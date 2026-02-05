#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

// High-End UI Colors for Terminal Branding
#define RED "\033[31m"
#define GRN "\033[32m"
#define YEL "\033[33m"
#define CYN "\033[36m"
#define RST "\033[0m"

// Algorithm: Lowercase helper to enable Case-Insensitive searching
string toLower(string s){
    for(int i = 0; i < s.length(); i++) if(s[i] >= 'A' && s[i] <= 'Z') s[i] += 32;
    return s;
}

// Data Structure: Struct to represent a Vehicle (The main Data object)
struct Car{
    string id, makeModel, trans, status;
    int year, hp, ts, stock, maxStock;
    double rate;
    // Constructor initializes car data and determines availability based on stock
    Car(string i, string m, int y, int h, int t, string tr, double r, int s){
        id = i; makeModel = m; year = y; hp = h; ts = t; trans = tr; rate = r; stock = s; maxStock = s; status = (stock > 0) ? "Available" : "No Stock";
    }
    // Logic: Formats vehicle specs into a professional showroom table row
    void displayRow() const {
        string s_txt = (stock > 0) ? to_string(stock) : "OUT";
        cout << "| " << left << setw(8) << id << "| " << left << setw(35) << makeModel << "| " << left << setw(5) << hp << "hp | " << left << setw(5) << trans << "| $" << left << setw(8) << rate << "| " << left << setw(5) << s_txt << "| " << left << setw(12) << status << " |" << endl;
    }
};

// Data Structure: Struct for Customer Database (Linked List Node)
struct Cust{
    string name, phone, carID;
    Cust* next;
    Cust(string n, string p, string c){name = n; phone = p; carID = c; next = NULL;}
};

// Data Structure: Basic Node used for the Showroom Linked List and Hash Table Chaining
struct Node{
    Car* data; Node* next;
    Node(Car* c){data = c; next = NULL;}
};

// Data Structure: Queue (Circular Array Implementation) for the Service Bay
class ServiceQueue{
    Car* queue[10]; int front, rear, size;
public:
    ServiceQueue(){front = 0; rear = -1; size = 0;}
    bool isFull(){return size == 10;}
    bool isEmpty(){return size == 0;}
    // Logic: Enqueue (FIFO) - Adds a returned car to the back of the wash bay queue
    void enqueue(Car* c){
        if(isFull()){cout << RED << "Bay Full!" << RST << endl; return;}
        rear = (rear + 1) % 10; queue[rear] = c; size++; c->status = "In-Service";
    }
    // Logic: Dequeue (FIFO) - Processes car from the front, cleaning it and restoring stock
    Car* dequeue(){
        if(isEmpty()){return NULL;}
        Car* c = queue[front]; front = (front + 1) % 10; size--; 
        if(c->stock < c->maxStock) c->stock++; 
        c->status = (c->stock > 0) ? "Available" : "No Stock"; return c;
    }
    // Logic: Traverses the circular array to display current queue status
    void display(){
        if(isEmpty()){cout << YEL << "Wash Bay Empty" << RST << endl; return;}
        cout << CYN << "\n--- WASH BAY QUEUE ---" << RST << endl;
        for(int i = 0, j = front; i < size; i++, j = (j + 1) % 10)
            cout << i + 1 << ". " << queue[j]->makeModel << " [" << queue[j]->id << "]" << endl;
    }
    // Integrity Check: Verifies if a specific ID is currently locked in service
    bool isInService(string id){
        for(int i = 0, j = front; i < size; i++, j = (j + 1) % 10)
            if(queue[j]->id == id) return true;
        return false;
    }
};

// Data Structure: Basic node for the History Stack
struct LogNode{
    string log; LogNode* next;
    LogNode(string s){log = s; next = NULL;}
};

// Data Structure: Stack (Linked List Implementation) for Activity Logs
class HistoryStack{
    LogNode* top;
public:
    HistoryStack(){top = NULL;}
    // Logic: Push (LIFO) - Adds most recent rental or search to the top of the stack
    void push(string s){LogNode* n = new LogNode(s); n->next = top; top = n;}
    // Logic: Stack Traversal - Displays all activity from newest to oldest
    void display(string title){
        LogNode* t = top; cout << CYN << "\n--- " << title << " ---" << RST << endl;
        if(!t){cout << "Empty" << endl; return;}
        while(t){cout << ">> " << t->log << endl; t = t->next;}
    }
};

// Data Structure: Hash Table for O(1) Search Complexity
class HashTable{
    Node* table[50];
public:
    HashTable(){for(int i = 0; i < 50; i++) table[i] = NULL;}
    // Logic: Hash Function - Converts Car ID string into an integer index (0-49)
    int hashFn(string id){
        int s = 0; for(unsigned i = 0; i < id.length(); i++) s += id[i];
        return s % 50;
    }
    // Logic: Insertion - Uses "Separate Chaining" to handle collisions at the same index
    void insert(Car* c){
        int idx = hashFn(c->id); Node* n = new Node(c); n->next = table[idx]; table[idx] = n;
    }
    // Logic: Search - Direct index lookup using the hash function for high speed
    Car* search(string id){
        Node* t = table[hashFn(id)];
        while(t){if(t->data->id == id) return t->data; t = t->next;}
        return NULL;
    }
    // Logic: Deletion - Removes the car from the hash box
    void remove(string id){
        int idx = hashFn(id); Node *t = table[idx], *p = NULL;
        while(t && t->data->id != id){p = t; t = t->next;}
        if(!t) return;
        if(!p) table[idx] = t->next; else p->next = t->next;
        delete t;
    }
};

// Data Structure: Main Linked List for Fleet Management (Showroom)
class Showroom{
    Node* head; HashTable* ht;
public:
    Showroom(HashTable* h){head = NULL; ht = h;}
    // Logic: Insertion - Appends a new vehicle to the end of the showroom list
    void add(Car* c){
        Node* n = new Node(c);
        if(!head){head = n;} else{Node* t = head; while(t->next) t = t->next; t->next = n;}
        ht->insert(c);
    }
    // Logic: Traversal - Walks through the entire list to display all vehicles
    void display(){
        cout << CYN << "--------------------------------------------------------------------------------------------------------" << endl;
        cout << "| ID      | Make & Model                       | Power  | TR   | Rate     | Stock| Status       |" << endl;
        cout << "--------------------------------------------------------------------------------------------------------" << RST << endl;
        Node* t = head; while(t){t->data->displayRow(); t = t->next;}
    }
    // Algorithm: Linear Search with case-insensitive matching
    void searchBrand(string k){
        Node* t = head; bool f = false; string lowerK = toLower(k);
        while(t){
            if(toLower(t->data->makeModel).find(lowerK) != string::npos){t->data->displayRow(); f = true;} 
            t = t->next;
        }
        if(!f) cout << RED << "No matches found." << RST << endl;
    }
    // Algorithm: Selection Sort - Rearranges list pointers based on Rate/Horsepower
    void sortByPrice(){
        for(Node* i = head; i; i = i->next)
            for(Node* j = i->next; j; j = j->next)
                if(i->data->rate < j->data->rate){Car* t = i->data; i->data = j->data; j->data = t;}
    }
    void sortByHP(){
        for(Node* i = head; i; i = i->next)
            for(Node* j = i->next; j; j = j->next)
                if(i->data->hp < j->data->hp){Car* t = i->data; i->data = j->data; j->data = t;}
    }
    // Logic: Admin Deletion - Safely removes vehicles while protecting memory integrity
    void del(string id, ServiceQueue& sq){
        Car* c = ht->search(id);
        if(!c){cout << RED << "ID not found." << RST << endl; return;}
        // Integrity Check: Car must be in showroom (stock full) and not in maintenance
        if(c->status == "In-Service" || sq.isInService(id) || c->stock < c->maxStock){
            cout << RED << "Cannot remove: Car is currently Rented or In-Service." << RST << endl; return;
        }
        Node *t = head, *p = NULL;
        while(t && t->data->id != id){p = t; t = t->next;}
        if(!p) head = t->next; else p->next = t->next;
        ht->remove(id); delete t->data; delete t; cout << YEL << "Vehicle Decommissioned." << RST << endl;
    }
    Car* get(string id){return ht->search(id);}
};

// UI: Branded Header Output
void header(){
    cout << CYN << "  _______ _    _  ______   _____             _____   _____    ____    _____  _  __" << endl;
    cout << " |__   __| |  | | |  ____| |  __ \\    /\\    |  __ \\ |  __ \\  / __ \\  / ____|| |/ /" << endl;
    cout << "    | |  | |__| | | |__    | |__) |  /  \\   | |  | || |  | || |  | || |     | ' / " << endl;
    cout << "    | |  |  __  | |  __|   |  ___/  / /\\ \\  | |  | || |  | || |  | || |     |  <  " << endl;
    cout << "    | |  | |  | | | |____  | |     / ____ \\ | |__| || |__| || |__| || |____ | . \\ " << endl;
    cout << "    |_|  |_|  |_| |______| |_|    /_/    \\_\\|_____/ |_____/  \\____/  \\_____||_|\\_\\" << endl;
    cout << "\n                       _____  _       _    _  ____  " << endl;
    cout << "                      / ____|| |     | |  | ||  _ \\ " << endl;
    cout << "                     | |     | |     | |  | || |_) |" << endl;
    cout << "                     | |     | |     | |  | ||  _ < " << endl;
    cout << "                     | |____ | |____ | |__| || |_) |" << endl;
    cout << "                      \\_____||______| \\____/ |____/ " << RST << endl;
    cout << "\n                      EXOTIC & PERFORMANCE CAR RENTALS" << endl;
    cout << "==================================================================================\n" << endl;
}

// Logic: Robust Input Sanitization
void clearInput(){cin.clear(); cin.ignore(1000, '\n');}

int main(){
    // Initializing all Data Structure objects
    HashTable ht; Showroom sr(&ht); ServiceQueue sq; HistoryStack rs, ss;
    Cust* custHead = NULL; string pass = "paddock77";

    // Population: Enthusiast fleet with custom rarity (stock) values
    sr.add(new Car("MB01", "Mercedes C218 CLS63", 2014, 577, 300, "Auto", 800, 2));
    sr.add(new Car("MB02", "Mercedes W204 C63 Black Series", 2012, 510, 300, "Auto", 1200, 1));
    sr.add(new Car("MB03", "Mercedes C197 SLS AMG Black Series", 2014, 622, 315, "Auto", 2500, 1));
    sr.add(new Car("MB04", "Mercedes R230 SL65 Black Series", 2009, 661, 320, "Auto", 3000, 1));
    sr.add(new Car("MB05", "Mercedes W222 S65 AMG", 2018, 621, 300, "Auto", 1500, 2));

    sr.add(new Car("BM01", "BMW F90 M5 CS", 2022, 627, 305, "Auto", 1800, 2));
    sr.add(new Car("BM02", "BMW G20 M3 CS", 2023, 543, 302, "Auto", 1400, 2));
    sr.add(new Car("BM03", "BMW F87 M2 CS", 2020, 444, 280, "Auto", 900, 2));

    sr.add(new Car("FE01", "Ferrari F12 Berlinetta", 2015, 730, 340, "Auto", 2200, 1));
        sr.add(new Car("FE02", "Ferrari 812 Competizione", 2022, 819, 340, "Auto", 4500, 1));
        sr.add(new Car("FE03", "Ferrari 360 Challenge Stradale", 2004, 420, 300, "F1", 1600, 1));
        sr.add(new Car("FE40", "Ferrari F40 (Icon)", 1987, 471, 324, "Man", 8000, 1));
        sr.add(new Car("RN01", "Renault Clio V6 Phase 2", 2005, 252, 245, "Man", 600, 1));
    sr.add(new Car("AL01", "Alpine A110s", 2023, 288, 260, "Auto", 550, 2));

    sr.add(new Car("MM01", "Mercedes-Maybach 62S Coupe", 2011, 604, 250, "Auto", 5000, 1));

    sr.add(new Car("PO01", "Porsche 911 (991.2) Turbo S", 2018, 580, 330, "Auto", 1700, 2));

    sr.add(new Car("TY01", "Toyota GR Yaris MT", 2025, 280, 230, "Man", 350, 5));
    sr.add(new Car("TY02", "Toyota GR Corolla MT", 2025, 300, 230, "Man", 380, 5));

    sr.add(new Car("AM01", "Aston Martin Vanquish Zagato", 2017, 580, 301, "Auto", 2800, 1));
    sr.add(new Car("AM02", "Aston Martin V12 Vanquish", 2015, 568, 323, "Auto", 1300, 2));

    sr.add(new Car("PR01", "Proton Satria Neo R3 Lotus", 2013, 145, 205, "Man", 150, 5));

    int c, age, days; string id, key, pInput;
    while(true){
        system("clear"); // UI Logic: Clears terminal for a modern app experience
        header();
        cout << "1. Showroom\n2. Search Car\n3. Sort Catalog\n4. Rent a Car\n5. Return Car\n6. Wash Bay\n7. History Logs\n8. Admin Mode\n0. Exit\nChoice: "; 
        if(!(cin >> c)){cout << RED << "Menu Input Error!" << RST << endl; clearInput(); continue;}
        if(c == 0) break;
        if(c == 1){
            sr.display(); char book; cout << "\nBook a car now? (y/n): "; cin >> book;
            if(book == 'y' || book == 'Y'){
                cout << "Enter Car ID: "; cin >> id; Car* car = sr.get(id);
                if(car && car->stock > 0){
                    cout << "Age: "; if(!(cin >> age) || age < 18){cout << RED << "Invalid Age!" << RST << endl; clearInput(); goto wait;}
                    // Safety Logic: Blocks high-power rentals for younger drivers
                    if(car->hp > 500 && age < 25){cout << RED << "Denied: Power level restricted." << RST << endl;}
                    else{
                        string name, phone; cout << "Name: "; cin.ignore(); getline(cin, name);
                        cout << "Phone: "; cin >> phone; cout << "Days: "; if(!(cin >> days) || days <= 0){cout << RED << "Invalid!" << RST << endl; clearInput(); goto wait;}
                        car->stock--; if(car->stock == 0) car->status = "Rented (Out)";
                        // Database Logic: Adds to Customer Linked List
                        Cust* nC = new Cust(name, phone, id); nC->next = custHead; custHead = nC;
                        rs.push("Rented " + car->makeModel + " to " + name);
                        cout << GRN << "\n--- RECEIPT ---\nCar: " << car->makeModel << "\nTotal: $" << car->rate*days << "\nApproved!" << RST << endl;
                    }
                } else cout << RED << "Unavailable!" << RST << endl;
            }
        }
        if(c == 2){cout << "Keyword: "; cin >> key; ss.push("Searched: " + key); sr.searchBrand(key);}
        if(c == 3){int s; cout << "Sort by: [1.Price 2.HP]: "; cin >> s; if(s == 1) sr.sortByPrice(); else sr.sortByHP(); sr.display();}
        if(c == 4){
            cout << "Car ID: "; cin >> id; Car* car = sr.get(id);
            if(car && car->stock > 0){
                cout << "Age: "; if(!(cin >> age) || age < 18){cout << RED << "Invalid Age (Min 18)!" << RST << endl; clearInput(); continue;}
                if(car->hp > 500 && age < 25){cout << RED << "Insurance Denied: High-power car requires age 25+." << RST << endl;}
                else{
                    string name, phone; cout << "Name: "; cin.ignore(); getline(cin, name);
                    cout << "Phone: "; cin >> phone;
                    cout << "Days: "; if(!(cin >> days) || days <= 0){cout << RED << "Invalid rental period!" << RST << endl; clearInput(); continue;}
                    car->stock--; if(car->stock == 0) car->status = "Rented (Out)";
                    Cust* nC = new Cust(name, phone, id); nC->next = custHead; custHead = nC;
                    rs.push("Rented " + car->makeModel + " to " + name);
                    cout << GRN << "\n--- RECEIPT ---\nCar: " << car->makeModel << "\nTotal: $" << car->rate*days << "\nApproved!" << RST << endl;
                }
            } else cout << RED << "Error: No Stock or Invalid ID!" << RST << endl;
        }
        if(c == 5){
            cout << "Car ID: "; cin >> id; Car* car = sr.get(id);
            if(car && car->stock < car->maxStock && car->status != "In-Service"){
                sq.enqueue(car); rs.push("Returned " + car->makeModel); cout << GRN << "Sent to Wash Bay" << RST << endl;
            } else cout << RED << "Error: Car is not currently rented or already in service." << RST << endl;
        }
        if(c == 6){sq.display(); if(!sq.isEmpty()){char y; cout << "Process car? (y/n): "; cin >> y; if(y == 'y') sq.dequeue();}}
        if(c == 7){rs.display("RENTAL LOGS"); ss.display("SEARCH HISTORY");}
        if(c == 8){
            cout << "Admin Password: "; cin >> pInput;
            if(pInput == pass){
                int a; cout << "\n--- ADMIN PANEL ---\n1. Add New Vehicle\n2. Decommission Vehicle\n3. View Customer Database\nChoice: "; cin >> a;
                if(a == 1){string i, m, t; int y, h, ts, s; double r; cout << "ID Model Year HP TS Trans Rate Stock: "; cin >> i >> m >> y >> h >> ts >> t >> r >> s; sr.add(new Car(i, m, y, h, ts, t, r, s));}
                if(a == 2){cout << "ID to Remove: "; cin >> id; sr.del(id, sq);}
                if(a == 3){
                    cout << CYN << "\n--- CUSTOMER DATABASE ---" << RST << endl;
                    Cust* tC = custHead; if(!tC) cout << "No records found." << endl;
                    while(tC){cout << "Name: " << left << setw(15) << tC->name << " | Phone: " << setw(12) << tC->phone << " | Car: " << tC->carID << endl; tC = tC->next;}
                }
            } else cout << RED << "Access Denied!" << RST << endl;
        }
        wait:
        cout << "\n(Enter to continue...)"; cin.ignore(); cin.get();
    }
    return 0;
}