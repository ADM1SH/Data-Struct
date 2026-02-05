#include <iostream>
#include <iomanip>

using namespace std;

struct Car{
    string id, makeModel, trans, status;
    int year, hp, ts;
    double rate;

    Car(string i, string m, int y, int h, int t, string tr, double r){
        id = i; makeModel = m; year = y; hp = h; ts = t; trans = tr; rate = r; status = "Available";
    }

    void displayRow() const {
        cout << "| " << left << setw(8) << id << "| " << left << setw(35) << makeModel << "| " << left << setw(6) << year << "| " << left << setw(5) << hp << "| " << left << setw(5) << ts << "| " << left << setw(5) << trans << "| $" << left << setw(8) << rate << "| " << left << setw(12) << status << " |" << endl;
    }
};

struct Node{
    Car* data;
    Node* next;
    Node(Car* c){data = c; next = NULL;}
};

class ServiceQueue{
    Car* queue[10];
    int front, rear, size;
public:
    ServiceQueue(){front = 0; rear = -1; size = 0;}
    bool isFull(){return size == 10;}
    bool isEmpty(){return size == 0;}
    void enqueue(Car* c){
        if(isFull()){cout << "Bay Full!" << endl; return;}
        rear = (rear + 1) % 10; queue[rear] = c; size++; c->status = "In-Service";
    }
    Car* dequeue(){
        if(isEmpty()){return NULL;}
        Car* c = queue[front]; front = (front + 1) % 10; size--; c->status = "Available"; return c;
    }
    void display(){
        if(isEmpty()){cout << "Bay Empty" << endl; return;}
        for(int i = 0, j = front; i < size; i++, j = (j + 1) % 10)
            cout << i + 1 << ". " << queue[j]->makeModel << " (" << queue[j]->id << ")" << endl;
    }
};

struct LogNode{
    string log;
    LogNode* next;
    LogNode(string s){log = s; next = NULL;}
};

class RentalStack{
    LogNode* top;
public:
    RentalStack(){top = NULL;}
    void push(string s){LogNode* n = new LogNode(s); n->next = top; top = n;}
    void display(){
        LogNode* t = top;
        if(!t){cout << "No Logs" << endl; return;}
        while(t){cout << ">> " << t->log << endl; t = t->next;}
    }
};

class HashTable{
    Node* table[50];
public:
    HashTable(){for(int i = 0; i < 50; i++) table[i] = NULL;}
    int hashFn(string id){
        int s = 0;
        for(int i = 0; i < id.length(); i++) s += id[i];
        return s % 50;
    }
    void insert(Car* c){
        int idx = hashFn(c->id);
        Node* n = new Node(c); n->next = table[idx]; table[idx] = n;
    }
    Car* search(string id){
        Node* t = table[hashFn(id)];
        while(t){if(t->data->id == id) return t->data; t = t->next;}
        return NULL;
    }
    void remove(string id){
        int idx = hashFn(id);
        Node *t = table[idx], *p = NULL;
        while(t && t->data->id != id){p = t; t = t->next;}
        if(!t) return;
        if(!p) table[idx] = t->next; else p->next = t->next;
        delete t;
    }
};

class Showroom{
    Node* head;
    HashTable* ht;
public:
    Showroom(HashTable* h){head = NULL; ht = h;}
    void add(Car* c){
        Node* n = new Node(c);
        if(!head){head = n;} else{Node* t = head; while(t->next) t = t->next; t->next = n;}
        ht->insert(c);
    }
    void display(){
        cout << "--------------------------------------------------------------------------------------------------------" << endl;
        cout << "| ID      | Make & Model                       | Year  | HP   | KPH  | TR   | Rate     | Status       |" << endl;
        cout << "--------------------------------------------------------------------------------------------------------" << endl;
        Node* t = head; while(t){t->data->displayRow(); t = t->next;}
    }
    void searchBrand(string k){
        Node* t = head; bool f = false;
        while(t){if(t->data->makeModel.find(k) != string::npos){t->data->displayRow(); f = true;} t = t->next;}
        if(!f) cout << "Not found" << endl;
    }
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
    void del(string id){
        Node *t = head, *p = NULL;
        while(t && t->data->id != id){p = t; t = t->next;}
        if(!t){cout << "Not found" << endl; return;}
        if(!p) head = t->next; else p->next = t->next;
        ht->remove(id); delete t->data; delete t; cout << "Deleted" << endl;
    }
    Car* get(string id){return ht->search(id);}
};

void header(){
    cout << "  _______ _    _  ______   _____             _____   _____    ____    _____  _  __" << endl;
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
    cout << "                      \\_____||______| \\____/ |____/ " << endl;
    cout << "\n                      EXOTIC & PERFORMANCE CAR RENTALS" << endl;
    cout << "==================================================================================\n" << endl;
}

int main(){
    HashTable ht; Showroom sr(&ht); ServiceQueue sq; RentalStack rs;
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

    int c, age; string id, key;
    while(true){
        header();
        cout << "1. Showroom\n2. Search Car\n3. Sort Catalog\n4. Rent a Car\n5. Return Car\n6. Wash Bay\n7. Recent Logs\n8. Admin Mode\n0. Exit\n\nChoice: "; cin >> c;
        if(c == 0) break;
        if(c == 1) sr.display();
        if(c == 2){cout << "Enter Keyword: "; cin >> key; sr.searchBrand(key);}
        if(c == 3){int s; cout << "Sort by [1.Price 2.HP]: "; cin >> s; if(s == 1) sr.sortByPrice(); else sr.sortByHP(); sr.display();}
        if(c == 4){
            cout << "Car ID: "; cin >> id; Car* car = sr.get(id);
            if(car && car->status == "Available"){
                cout << "Driver Age: "; cin >> age;
                if(car->hp > 500 && age < 25){cout << "\n>>> ACCESS DENIED: Driver age 25+ required for 500+ HP models." << endl;}
                else{car->status = "Rented"; rs.push("Rented " + car->makeModel); cout << "\n>>> APPROVED! Keys issued." << endl;}
            } else{cout << "\n>>> Error: Car unavailable or invalid ID." << endl;}
        }
        if(c == 5){cout << "Car ID: "; cin >> id; Car* car = sr.get(id); if(car && car->status == "Rented"){sq.enqueue(car); rs.push("Returned " + car->makeModel); cout << "\n>>> Car sent to Service Bay." << endl;} else cout << "\n>>> Error: ID not rented." << endl;}
        if(c == 6){sq.display(); if(!sq.isEmpty()){char y; cout << "Finish wash? (y/n): "; cin >> y; if(y == 'y') sq.dequeue();}}
        if(c == 7) rs.display();
        if(c == 8){
            int a; cout << "Admin: 1.Add 2.Delete: "; cin >> a;
            if(a == 1){string i, m, t; int y, h, ts; double r; cout << "ID Model Year HP TS Trans Rate: "; cin >> i >> m >> y >> h >> ts >> t >> r; sr.add(new Car(i, m, y, h, ts, t, r));}
            if(a == 2){cout << "Enter ID to delete: "; cin >> id; sr.del(id);}
        }
        cout << "\n(Press Enter to continue...)"; cin.ignore(); cin.get();
    }
    return 0;
}
