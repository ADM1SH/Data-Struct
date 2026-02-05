#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

// High-End UI Colors
#define RED "\033[31m"
#define GRN "\033[32m"
#define YEL "\033[33m"
#define CYN "\033[36m"
#define RST "\033[0m"

// Lowercase helper for case-insensitive search
string toLower(string s){
    for(int i = 0; i < s.length(); i++) if(s[i] >= 'A' && s[i] <= 'Z') s[i] += 32;
    return s;
}

// Structure to hold car information
struct Car{
    string id, makeModel, trans, status;
    int year, hp, ts, stock, maxStock;
    double rate;
    // Constructor: status depends on initial stock
    Car(string i, string m, int y, int h, int t, string tr, double r, int s){
        id = i; makeModel = m; year = y; hp = h; ts = t; trans = tr; rate = r; stock = s; maxStock = s; status = (stock > 0) ? "Available" : "No Stock";
    }
    // Formats car details for the catalog view
    void displayRow() const {
        string s_txt = (stock > 0) ? to_string(stock) : "OUT";
        cout << "| " << left << setw(8) << id << "| " << left << setw(35) << makeModel << "| " << left << setw(5) << hp << "hp | " << left << setw(5) << trans << "| $" << left << setw(8) << rate << "| " << left << setw(5) << s_txt << "| " << left << setw(12) << status << " |" << endl;
    }
};

// Customer database entry for the admin list
struct Cust{
    string name, phone, carID;
    Cust* next;
    Cust(string n, string p, string c){name = n; phone = p; carID = c; next = NULL;}
};

// Generic linked list node
struct Node{
    Car* data; Node* next;
    Node(Car* c){data = c; next = NULL;}
};

// Queue (Circular Array) for processing returned cars
class ServiceQueue{
    Car* queue[10]; int front, rear, size;
public:
    ServiceQueue(){front = 0; rear = -1; size = 0;}
    bool isFull(){return size == 10;}
    bool isEmpty(){return size == 0;}
    // Enqueue: Car enters service bay after rental
    void enqueue(Car* c){
        if(isFull()){cout << RED << "Bay Full!" << RST << endl; return;}
        rear = (rear + 1) % 10; queue[rear] = c; size++; c->status = "In-Service";
    }
    // Dequeue: Car is cleaned and stock is restored
    Car* dequeue(){
        if(isEmpty()){return NULL;}
        Car* c = queue[front]; front = (front + 1) % 10; size--;
        if(c->stock < c->maxStock) c->stock++;
        c->status = (c->stock > 0) ? "Available" : "No Stock"; return c;
    }
    void display(){
        if(isEmpty()){cout << YEL << "Wash Bay Empty" << RST << endl; return;}
        cout << CYN << "\n--- WASH BAY QUEUE ---" << RST << endl;
        for(int i = 0, j = front; i < size; i++, j = (j + 1) % 10)
            cout << i + 1 << ". " << queue[j]->makeModel << " [" << queue[j]->id << "]" << endl;
    }
    // Check if a specific car is currently in the wash bay
    bool isInService(string id){
        for(int i = 0, j = front; i < size; i++, j = (j + 1) % 10)
            if(queue[j]->id == id) return true;
        return false;
    }
};

// Simple node for activity history
struct LogNode{
    string log; LogNode* next;
    LogNode(string s){log = s; next = NULL;}
};

// Stack (Linked List) for LIFO activity tracking
class HistoryStack{
    LogNode* top;
public:
    HistoryStack(){top = NULL;}
    // Push: Adds most recent activity to top
    void push(string s){LogNode* n = new LogNode(s); n->next = top; top = n;}
    void display(string title){
        LogNode* t = top; cout << CYN << "\n--- " << title << " ---" << RST << endl;
        if(!t){cout << "Empty" << endl; return;}
        while(t){cout << ">> " << t->log << endl; t = t->next;}
    }
};

// Hash Table with separate chaining for O(1) searches
class HashTable{
    Node* table[50];
public:
    HashTable(){for(int i = 0; i < 50; i++) table[i] = NULL;}
    int hashFn(string id){
        int s = 0; for(unsigned i = 0; i < id.length(); i++) s += id[i];
        return s % 50;
    }
    void insert(Car* c){
        int idx = hashFn(c->id); Node* n = new Node(c); n->next = table[idx]; table[idx] = n;
    }
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

// Main Inventory Linked List
class Showroom{
    Node* head; HashTable* ht;
public:
    Showroom(HashTable* h){head = NULL; ht = h;}
    void add(Car* c){
        Node* n = new Node(c);
        if(!head){head = n;} else{Node* t = head; while(t->next) t = t->next; t->next = n;}
        ht->insert(c);
    }
    void display(){
        cout << CYN << "--------------------------------------------------------------------------------------------------------" << endl;
        cout << "| ID      | Make & Model                       | Power  | TR   | Rate     | Stock| Status       |" << endl;
        cout << "--------------------------------------------------------------------------------------------------------" << RST << endl;
        Node* t = head; while(t){t->data->displayRow(); t = t->next;}
    }
    // Case-insensitive search: keyword matching in make/model
    void searchBrand(string k){
        Node* t = head; bool f = false; string lowerK = toLower(k);
        while(t){
            if(toLower(t->data->makeModel).find(lowerK) != string::npos){t->data->displayRow(); f = true;} 
            t = t->next;
        }
        if(!f) cout << RED << "No matches found." << RST << endl;
    }
    // Selection Sort for Descending Price/HP
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
    void del(string id, ServiceQueue& sq){
        Car* c = ht->search(id);
        if(!c){cout << RED << "ID not found." << RST << endl; return;}
        // Integrity Guard: Don't delete cars in service or rented
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

// Branded Header ASCII
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

void clearInput(){cin.clear(); cin.ignore(1000, '\n');}

int main(){
    HashTable ht; Showroom sr(&ht); ServiceQueue sq; HistoryStack rs, ss;
    Cust* custHead = NULL; string pass = "paddock77";

    // Default Enthusiast Fleet with Rarity levels
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
        header();
        cout << "1. Showroom\n2. Search Car\n3. Sort Catalog\n4. Rent a Car\n5. Return Car\n6. Wash Bay\n7. History Logs\n8. Admin Mode\n0. Exit\nChoice: "; 
        if(!(cin >> c)){cout << RED << "Menu Input Error!" << RST << endl; clearInput(); continue;}
        if(c == 0) break;
        if(c == 1) sr.display();
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
        cout << "\n(Enter to continue...)\n"; cin.ignore(); cin.get();
    }
    return 0;
}