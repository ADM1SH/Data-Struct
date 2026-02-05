#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

// ANSI Escape Codes for Terminal Colors
#define RED "\033[31m"
#define GRN "\033[32m"
#define YEL "\033[33m"
#define CYN "\033[36m"
#define RST "\033[0m"

// Structure to represent a Car object
struct Car{
    string id, makeModel, trans, status;
    int year, hp, ts;
    double rate;
    Car(string i, string m, int y, int h, int t, string tr, double r){
        id = i; makeModel = m; year = y; hp = h; ts = t; trans = tr; rate = r; status = "Available";
    }
    // Formats car data into a clean table row
    void displayRow() const {
        cout << "| " << left << setw(8) << id << "| " << left << setw(35) << makeModel << "| " << left << setw(6) << year << "| " << left << setw(5) << hp << "| " << left << setw(5) << ts << "| " << left << setw(5) << trans << "| $" << left << setw(8) << rate << "| " << left << setw(12) << status << " |" << endl;
    }
};

// Node for Linked List and Separate Chaining in Hash Table
struct Node{
    Car* data; Node* next;
    Node(Car* c){data = c; next = NULL;}
};

// Queue Implementation (Circular Array) for the Wash Bay
class ServiceQueue{
    Car* queue[10]; int front, rear, size;
public:
    ServiceQueue(){front = 0; rear = -1; size = 0;}
    bool isFull(){return size == 10;}
    bool isEmpty(){return size == 0;}
    // FIFO: Adds car to the back of the queue
    void enqueue(Car* c){
        if(isFull()){cout << RED << "Bay Full!" << RST << endl; return;}
        rear = (rear + 1) % 10; queue[rear] = c; size++; c->status = "In-Service";
    }
    // FIFO: Removes car from the front after service
    Car* dequeue(){
        if(isEmpty()){return NULL;}
        Car* c = queue[front]; front = (front + 1) % 10; size--; c->status = "Available"; return c;
    }
    void display(){
        if(isEmpty()){cout << YEL << "Wash Bay Empty" << RST << endl; return;}
        cout << CYN << "\n--- WASH BAY QUEUE ---" << RST << endl;
        for(int i = 0, j = front; i < size; i++, j = (j + 1) % 10)
            cout << i + 1 << ". " << queue[j]->makeModel << " [" << queue[j]->id << "]" << endl;
    }
};

// Node for Stack History
struct LogNode{
    string log; LogNode* next;
    LogNode(string s){log = s; next = NULL;}
};

// Stack Implementation (Linked List) for Activity Logs
class HistoryStack{
    LogNode* top;
public:
    HistoryStack(){top = NULL;}
    // LIFO: Pushes new activity to the top of the stack
    void push(string s){LogNode* n = new LogNode(s); n->next = top; top = n;}
    // LIFO: Displays logs starting from the most recent
    void display(string title){
        LogNode* t = top; cout << CYN << "\n--- " << title << " ---" << RST << endl;
        if(!t){cout << "Empty" << endl; return;}
        while(t){cout << ">> " << t->log << endl; t = t->next;}
    }
};

// Hash Table for O(1) Search Complexity
class HashTable{
    Node* table[50];
public:
    HashTable(){for(int i = 0; i < 50; i++) table[i] = NULL;}
    // Simple ASCII sum hash function to map IDs to indexes
    int hashFn(string id){
        int s = 0; for(int i = 0; i < id.length(); i++) s += id[i];
        return s % 50;
    }
    void insert(Car* c){
        int idx = hashFn(c->id); Node* n = new Node(c); n->next = table[idx]; table[idx] = n;
    }
    // Direct lookup by hashing the key
    Car* search(string id){
        Node* t = table[hashFn(id)];
        while(t){if(t->data->id == id) return t->data; t = t->next;}
        return NULL;
    }
    void remove(string id){
        int idx = hashFn(id); Node *t = table[idx], *p = NULL;
        while(t && t->data->id != id){p = t; t = t->next;}
        if(!t) return;
        if(!p) table[idx] = t->next; else p->next = t->next;
        delete t;
    }
};

// Linked List Class for the Main Showroom Fleet
class Showroom{
    Node* head; HashTable* ht;
public:
    Showroom(HashTable* h){head = NULL; ht = h;}
    // Adds a new car to the end of the Linked List
    void add(Car* c){
        Node* n = new Node(c);
        if(!head){head = n;} else{Node* t = head; while(t->next) t = t->next; t->next = n;}
        ht->insert(c);
    }
    void display(){
        cout << CYN << "--------------------------------------------------------------------------------------------------------" << endl;
        cout << "| ID      | Make & Model                       | Year  | HP   | KPH  | TR   | Rate     | Status       |" << endl;
        cout << "--------------------------------------------------------------------------------------------------------" << RST << endl;
        Node* t = head; while(t){t->data->displayRow(); t = t->next;}
    }
    // Linear Search: Traverses the list to find matching keywords
    void searchBrand(string k){
        Node* t = head; bool f = false;
        while(t){if(t->data->makeModel.find(k) != string::npos){t->data->displayRow(); f = true;} t = t->next;}
        if(!f) cout << RED << "No matches found." << RST << endl;
    }
    // Selection Sort: Sorts cars by price in descending order
    void sortByPrice(){
        for(Node* i = head; i; i = i->next)
            for(Node* j = i->next; j; j = j->next)
                if(i->data->rate < j->data->rate){Car* t = i->data; i->data = j->data; j->data = t;}
        cout << GRN << "Sorted by Price (Desc)" << RST << endl;
    }
    // Selection Sort: Sorts cars by Horsepower in descending order
    void sortByHP(){
        for(Node* i = head; i; i = i->next)
            for(Node* j = i->next; j; j = j->next)
                if(i->data->hp < j->data->hp){Car* t = i->data; i->data = j->data; j->data = t;}
        cout << GRN << "Sorted by Horsepower (Desc)" << RST << endl;
    }
    // Deletes car from Linked List and Hash Table
    void del(string id){
        Node *t = head, *p = NULL;
        while(t && t->data->id != id){p = t; t = t->next;}
        if(!t){cout << RED << "Error: ID [" << id << "] not found." << RST << endl; return;}
        if(!p) head = t->next; else p->next = t->next;
        ht->remove(id); delete t->data; delete t; cout << YEL << "Car Removed." << RST << endl;
    }
    Car* get(string id){return ht->search(id);}
};

// Displays the branded ASCII Logo
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

// Generates a professional receipt for the customer
void printReceipt(Car* c, int age, int days){
    cout << GRN << "\n========================================" << endl;
    cout << "          THE PADDOCK CLUB RECEIPT" << endl;
    cout << "========================================" << endl;
    cout << "Car:    " << c->makeModel << endl;
    cout << "Year:   " << c->year << " (" << c->hp << " HP)" << endl;
    cout << "Driver: Age " << age << endl;
    cout << "Period: " << days << " Day(s)" << endl;
    cout << "----------------------------------------" << endl;
    cout << "TOTAL:  $" << fixed << setprecision(2) << c->rate * days << endl;
    cout << "========================================" << RST << endl;
}

// Handles invalid inputs (e.g., characters instead of numbers)
void clearInput(){cin.clear(); cin.ignore(1000, '\n');}

int main(){
    HashTable ht; Showroom sr(&ht); ServiceQueue sq; HistoryStack rs, ss;
    // Initializing the enthusiast fleet
    sr.add(new Car("MB01", "Mercedes C218 CLS63", 2014, 577, 300, "Auto", 800));
    sr.add(new Car("MB02", "Mercedes W204 C63 Black Series", 2012, 510, 300, "Auto", 1200));
    sr.add(new Car("MB03", "Mercedes C197 SLS AMG Black Series", 2014, 622, 315, "Auto", 2500));
    sr.add(new Car("MB04", "Mercedes R230 SL65 Black Series", 2009, 661, 320, "Auto", 3000));
    sr.add(new Car("MB05", "Mercedes W222 S65 AMG", 2018, 621, 300, "Auto", 1500));
    sr.add(new Car("BM01", "BMW F90 M5 CS", 2022, 627, 305, "Auto", 1800));
    sr.add(new Car("BM02", "BMW G20 M3 CS", 2023, 543, 302, "Auto", 1400));
    sr.add(new Car("BM03", "BMW F87 M2 CS", 2020, 444, 280, "Auto", 900));
    sr.add(new Car("FE01", "Ferrari F12 Berlinetta", 2015, 730, 340, "Auto", 2200));
    sr.add(new Car("FE02", "Ferrari 812 Competizione", 2022, 819, 340, "Auto", 4500));
    sr.add(new Car("FE03", "Ferrari 360 Challenge Stradale", 2004, 420, 300, "F1", 1600));
    sr.add(new Car("RN01", "Renault Clio V6 Phase 2", 2005, 252, 245, "Man", 600));
    sr.add(new Car("AL01", "Alpine A110s", 2023, 288, 260, "Auto", 550));
    sr.add(new Car("MM01", "Mercedes-Maybach 62S Coupe", 2011, 604, 250, "Auto", 5000));
    sr.add(new Car("PO01", "Porsche 911 (991.2) Turbo S", 2018, 580, 330, "Auto", 1700));
    sr.add(new Car("TY01", "Toyota GR Yaris MT", 2025, 280, 230, "Man", 350));
    sr.add(new Car("TY02", "Toyota GR Corolla MT", 2025, 300, 230, "Man", 380));
    sr.add(new Car("AM01", "Aston Martin Vanquish Zagato", 2017, 580, 301, "Auto", 2800));
    sr.add(new Car("AM02", "Aston Martin V12 Vanquish", 2015, 568, 323, "Auto", 1300));
    sr.add(new Car("PR01", "Proton Satria Neo R3 Lotus", 2013, 145, 205, "Man", 150));

    int c, age, days; string id, key;
    while(true){
        header();
        cout << "1. Showroom\n2. Search Car\n3. Sort Catalog\n4. Rent a Car\n5. Return Car\n6. Wash Bay\n7. History Logs\n8. Admin Mode\n0. Exit\n\nChoice: "; 
        if(!(cin >> c)){cout << RED << "Invalid choice!" << RST << endl; clearInput(); continue;}
        if(c == 0) break;
        if(c == 1) sr.display();
        if(c == 2){cout << "Keyword: "; cin >> key; ss.push("Searched: " + key); sr.searchBrand(key);}
        if(c == 3){int s; cout << "1.Price 2.HP: "; cin >> s; if(s == 1) sr.sortByPrice(); else sr.sortByHP(); sr.display();}
        if(c == 4){
            cout << "Car ID: "; cin >> id; Car* car = sr.get(id);
            if(car && car->status == "Available"){
                cout << "Age: "; if(!(cin >> age)){cout << RED << "Invalid age!" << RST << endl; clearInput(); continue;}
                // High-performance safety check logic
                if(car->hp > 500 && age < 25){cout << RED << "Too powerful for your age! 25+ required." << RST << endl;}
                else{
                    cout << "Days: "; if(!(cin >> days)){cout << RED << "Invalid days!" << RST << endl; clearInput(); continue;}
                    car->status = "Rented"; rs.push("Rented " + car->makeModel); printReceipt(car, age, days);
                }
            } else{cout << RED << "Unavailable or invalid ID." << RST << endl;}
        }
        if(c == 5){cout << "Car ID: "; cin >> id; Car* car = sr.get(id); if(car && car->status == "Rented"){sq.enqueue(car); rs.push("Returned " + car->makeModel); cout << GRN << "Sent to Wash Bay." << RST << endl;} else cout << RED << "Error: ID not found or not currently rented." << RST << endl;}
        if(c == 6){sq.display(); if(!sq.isEmpty()){char y; cout << "Finish wash? (y/n): "; cin >> y; if(y == 'y') sq.dequeue();}}
        if(c == 7){rs.display("RENTAL LOGS"); ss.display("SEARCH HISTORY");}
        if(c == 8){
            int a; cout << "Admin: 1.Add 2.Delete: "; cin >> a;
            if(a == 1){string i, m, t; int y, h, ts; double r; cout << "ID Model Year HP TS Trans Rate: "; cin >> i >> m >> y >> h >> ts >> t >> r; sr.add(new Car(i, m, y, h, ts, t, r));}
            if(a == 2){cout << "Enter ID to delete: "; cin >> id; sr.del(id);}
        }
        cout << "\n(Enter to continue...)"; cin.ignore(); cin.get();
    }
    return 0;
}