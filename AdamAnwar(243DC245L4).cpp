#include <iostream>

using namespace std;

// High-End UI Colors
#define RED "\033[31m"
#define GRN "\033[32m"
#define YEL "\033[33m"
#define CYN "\033[36m"
#define RST "\033[0m"

// Algorithm: Lowercase helper for search
string toLower(string s){
    for(int i = 0; i < s.length(); i++) if(s[i] >= 'A' && s[i] <= 'Z') s[i] += 32;
    return s;
}

// Helper to manually print spaces for table alignment (No iomanip)
void printSpace(int width, string s){
    int len = s.length();
    for(int i = 0; i < (width - len); i++) cout << " ";
}

void printSpaceInt(int width, int n){
    string s = to_string(n);
    for(int i = 0; i < (width - s.length()); i++) cout << " ";
}

// Data Structure: Car Struct
struct Car{
    string id, makeModel, trans, status;
    int year, hp, ts, stock, maxStock; 
    double rate;
    Car(string i, string m, int y, int h, int t, string tr, double r, int s){
        id = i; makeModel = m; year = y; hp = h; ts = t; trans = tr; rate = r; stock = s; maxStock = s; status = (stock > 0) ? "Available" : "No Stock";
    }
    void displayRow() const {
        string s_txt = (stock > 0) ? to_string(stock) : "OUT";
        cout << "| " << id; printSpace(8, id);
        cout << "| " << makeModel; printSpace(35, makeModel);
        cout << "| " << hp << "hp "; printSpaceInt(5, hp);
        cout << "| " << trans; printSpace(5, trans);
        cout << "| $" << (int)rate; printSpaceInt(8, (int)rate);
        cout << "| " << s_txt; printSpace(5, s_txt);
        cout << "| " << status; printSpace(12, status);
        cout << " |" << endl;
    }
};

// Data Structure: Linked List for Customers
struct Cust{
    string name, phone, carID;
    Cust* next;
    Cust(string n, string p, string c){name = n; phone = p; carID = c; next = NULL;}
};

// Queue Data Structure: Circular Array Implementation (Wash Bay)
class ServiceQueue{
    Car* queue[10]; int front, rear, size;
public:
    ServiceQueue(){front = 0; rear = -1; size = 0;}
    bool isFull(){return size == 10;}
    bool isEmpty(){return size == 0;}
    void enqueue(Car* c){
        if(isFull()){cout << RED << "Bay Full!" << RST << endl; return;}
        rear = (rear + 1) % 10; queue[rear] = c; size++; c->status = "In-Service";
    }
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
};

// Stack Data Structure: Linked List Implementation (Logs)
struct LogNode{
    string log; LogNode* next;
    LogNode(string s){log = s; next = NULL;}
};

class HistoryStack{
    LogNode* top;
public:
    HistoryStack(){top = NULL;}
    void push(string s){LogNode* n = new LogNode(s); n->next = top; top = n;}
    void display(string title){
        LogNode* t = top; cout << CYN << "\n--- " << title << " ---" << RST << endl;
        if(!t){cout << "Empty" << endl; return;}
        while(t){cout << ">> " << t->log << endl; t = t->next;}
    }
};

// Hash Table Data Structure: Quick Search O(1)
class HashTable{
    Car* table[50];
public:
    HashTable(){for(int i = 0; i < 50; i++) table[i] = NULL;}
    int hashFn(string id){
        int s = 0; for(int i = 0; i < id.length(); i++) s += id[i];
        return s % 50;
    }
    void insert(Car* c){table[hashFn(c->id)] = c;} // Simplified for lab style
    Car* search(string id){return table[hashFn(id)];}
};

// Showroom Implementation: Array of Pointers
class Showroom{
    Car* fleet[100]; // Max 100 cars
    int count;
    HashTable* ht;
public:
    Showroom(HashTable* h){count = 0; ht = h;}
    void add(Car* c){
        if(count < 100){fleet[count++] = c; ht->insert(c);}
    }
    void display(){
        cout << CYN << "--------------------------------------------------------------------------------------------------------" << endl;
        cout << "| ID      | Make & Model                       | Power  | TR   | Rate     | Stock| Status       |" << endl;
        cout << "--------------------------------------------------------------------------------------------------------" << RST << endl;
        for(int i = 0; i < count; i++) fleet[i]->displayRow();
    }
    // Search Algorithm: Linear Search
    void searchBrand(string k){
        bool f = false; string lowK = toLower(k);
        for(int i = 0; i < count; i++){
            if(toLower(fleet[i]->makeModel).find(lowK) != string::npos){fleet[i]->displayRow(); f = true;}
        }
        if(!f) cout << RED << "Not found." << RST << endl;
    }
    // Sorting Algorithm: Selection Sort
    void sortByPrice(){
        for(int i = 0; i < count-1; i++){
            for(int j = i+1; j < count; j++){
                if(fleet[i]->rate < fleet[j]->rate){Car* t = fleet[i]; fleet[i] = fleet[j]; fleet[j] = t;}
            }
        }
    }
    void sortByHP(){
        for(int i = 0; i < count-1; i++){
            for(int j = i+1; j < count; j++){
                if(fleet[i]->hp < fleet[j]->hp){Car* t = fleet[i]; fleet[i] = fleet[j]; fleet[j] = t;}
            }
        }
    }
    Car* get(string id){return ht->search(id);}
};

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

    // Initial Population
    sr.add(new Car("MB01", "Mercedes C218 CLS63", 2014, 577, 300, "Auto", 800, 2));
    sr.add(new Car("MB02", "Mercedes W204 C63 Black Series", 2012, 510, 300, "Auto", 1200, 1));
    sr.add(new Car("MB03", "Mercedes C197 SLS AMG Black Series", 2014, 622, 315, "Auto", 2500, 1));
    sr.add(new Car("FE40", "Ferrari F40 (Icon)", 1987, 471, 324, "Man", 8000, 1));
    sr.add(new Car("TY01", "Toyota GR Yaris MT", 2025, 280, 230, "Man", 350, 5));
    sr.add(new Car("PR01", "Proton Satria Neo R3 Lotus", 2013, 145, 205, "Man", 150, 5));

    int c, age, days; string id, key, pInput;
    while(true){
        header();
        cout << "1. Showroom\n2. Search Car\n3. Sort Catalog\n4. Rent a Car\n5. Return Car\n6. Wash Bay\n7. History Logs\n8. Admin Mode\n0. Exit\nChoice: "; 
        if(!(cin >> c)){cout << RED << "Error!" << RST << endl; clearInput(); continue;}
        if(c == 0) break;
        system("clear");
        if(c == 1){
            sr.display(); char book; cout << "\nBook a car now? (y/n): "; cin >> book;
            if(book == 'y' || book == 'Y'){
                cout << "ID: "; cin >> id; Car* car = sr.get(id);
                if(car && car->stock > 0){
                    cout << "Age: "; cin >> age;
                    if(car->hp > 500 && age < 25){cout << RED << "Denied!" << RST << endl;}
                    else {
                        string name, phone; cout << "Name: "; cin.ignore(); getline(cin, name);
                        cout << "Phone: "; cin >> phone; cout << "Days: "; cin >> days;
                        car->stock--; rs.push("Rented " + car->makeModel);
                        Cust* nC = new Cust(name, phone, id); nC->next = custHead; custHead = nC;
                        cout << GRN << "Approved!" << RST << endl;
                    }
                } else cout << RED << "Unavailable!" << RST << endl;
            }
        }
        if(c == 2){cout << "Keyword: "; cin >> key; ss.push("Searched: " + key); sr.searchBrand(key);}
        if(c == 3){int s; cout << "1.Price 2.HP: "; cin >> s; if(s == 1) sr.sortByPrice(); else sr.sortByHP(); sr.display();}
        if(c == 4){
            cout << "Car ID: "; cin >> id; Car* car = sr.get(id);
            if(car && car->stock > 0){
                cout << "Age: "; cin >> age;
                if(car->hp > 500 && age < 25){cout << RED << "Denied!" << RST << endl;}
                else {
                    string name, phone; cout << "Name: "; cin.ignore(); getline(cin, name);
                    cout << "Phone: "; cin >> phone; cout << "Days: "; cin >> days;
                    car->stock--; rs.push("Rented " + car->makeModel);
                    Cust* nC = new Cust(name, phone, id); nC->next = custHead; custHead = nC;
                    cout << GRN << "Approved!" << RST << endl;
                }
            } else cout << RED << "Unavailable!" << RST << endl;
        }
        if(c == 5){
            cout << "Car ID: "; cin >> id; Car* car = sr.get(id);
            if(car && car->stock < car->maxStock){sq.enqueue(car); rs.push("Returned " + car->makeModel);}
            else cout << RED << "Error!" << RST << endl;
        }
        if(c == 6){sq.display(); if(!sq.isEmpty()){char y; cout << "Finish? (y/n): "; cin >> y; if(y == 'y') sq.dequeue();}}
        if(c == 7){rs.display("RENTAL LOGS"); ss.display("SEARCH HISTORY");}
        if(c == 8){
            cout << "Password: "; cin >> pInput;
            if(pInput == pass){
                int a; cout << "1.Add 2.Customer List: "; cin >> a;
                if(a == 1){
                    string i, m, t; int y, h, ts, s; double r; 
                    cout << "ID: "; cin >> i; cout << "Model: "; cin.ignore(); getline(cin, m);
                    cout << "Year: "; cin >> y; cout << "HP: "; cin >> h; cout << "TS: "; cin >> ts;
                    cout << "Trans: "; cin >> t; cout << "Rate: "; cin >> r; cout << "Stock: "; cin >> s;
                    sr.add(new Car(i, m, y, h, ts, t, r, s));
                }
                if(a == 3 || a == 2){
                    Cust* tC = custHead; if(!tC) cout << "No records" << endl;
                    while(tC){cout << "Name: " << tC->name << " | Car: " << tC->carID << endl; tC = tC->next;}
                }
            } else cout << RED << "Denied!" << RST << endl;
        }
        cout << "\n(Enter to continue...)"; cin.ignore(); cin.get();
    }
    return 0;
}