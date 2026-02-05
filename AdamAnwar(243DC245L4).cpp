#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <thread>
#include <chrono>
#include <algorithm>

using namespace std;

// Ultimate UI Colors
#define RED "\033[1;31m"
#define GRN "\033[1;32m"
#define YEL "\033[1;33m"
#define CYN "\033[1;36m"
#define MAG "\033[1;35m"
#define RST "\033[0m"

// Utils: Time & String
string getTimestamp(){time_t now=time(0); char* dt=ctime(&now); string s(dt); return s.substr(0, s.length()-1);}
string toLower(string s){for(int i=0; i<s.length(); i++) s[i]=tolower(s[i]); return s;}
void loading(string msg){cout << YEL << msg << flush; for(int i=0;i<3;i++){this_thread::sleep_for(chrono::milliseconds(200)); cout << "." << flush;} cout << RST << endl;}

// Data Structure: Car Object
struct Car{
    string id, make, trans, status;
    int year, hp, stock, maxStock, rents;
    double baseRate;
    Car(string i, string m, int y, int h, int s, string t, double r, int rc=0){
        id=i; make=m; year=y; hp=h; stock=s; maxStock=s; trans=t; baseRate=r; rents=rc; status=(s>0)?"Available":"Out of Stock";
    }
    double getPrice(){return (stock == 1 && maxStock > 1) ? baseRate * 1.2 : baseRate;} 
    void display() const {
        double currRate = (stock == 1 && maxStock > 1) ? baseRate * 1.2 : baseRate;
        string rateStr = "$" + to_string((int)currRate) + ((currRate > baseRate) ? " (SURGE)" : "");
        cout << "| " << left << setw(6) << id << "| " << setw(32) << make << "| " << setw(5) << hp << "| " << setw(15) << rateStr << "| " << setw(5) << stock << "| " << setw(12) << status << "|" << endl;
    }
};

// Sorting Helpers
bool comparePrice(Car* a, Car* b){return a->getPrice() > b->getPrice();}
bool compareHP(Car* a, Car* b){return a->hp > b->hp;}

// Data Structure: Customer Node (Linked List)
struct Cust{
    string name, phone, carID, tier, date;
    double spent;
    Cust* next;
    Cust(string n, string p, string c, double s, string d){
        name=n; phone=p; carID=c; spent=s; date=d; next=NULL;
        tier = (spent > 5000) ? "PLATINUM" : (spent > 2000) ? "GOLD" : "STANDARD";
    }
};

// Data Structure: Queue (Circular Array) for Service Bay
class ServiceQueue{
    Car* q[10]; int f, r, sz;
public:
    ServiceQueue(){f=0; r=-1; sz=0;}
    void add(Car* c){if(sz==10){cout<<RED<<"Bay Full!"<<RST<<endl;return;} r=(r+1)%10; q[r]=c; sz++; c->status="Servicing";}
    void process(){if(sz==0)return; Car* c=q[f]; f=(f+1)%10; sz--; if(c->stock<c->maxStock) c->stock++; c->status=(c->stock>0)?"Available":"Out"; cout<<GRN<<"Service Complete: "<<c->make<<RST<<endl;}
    void show(){if(sz==0)cout<<YEL<<"[Empty]"<<RST<<endl; else for(int i=0,j=f; i<sz; i++,j=(j+1)%10) cout<<i+1<<". "<<q[j]->make<<endl;}
};

// Data Structure: Stack (Linked List) for Audit Logs (NOW PERSISTENT)
struct Log{string msg, time; Log* next; Log(string m){msg=m; time=getTimestamp(); next=NULL;}};
class AuditLog{
    Log* top;
public:
    AuditLog(){top=NULL;}
    void push(string m){
        Log* n=new Log(m); n->next=top; top=n;
        ofstream f("audit_log.txt", ios::app); // Appends every action to a text file
        f << "[" << n->time << "] " << m << endl; f.close();
    }
    void show(){Log* t=top; if(!t) cout<<YEL<<"No logs in session."<<RST<<endl; while(t){cout<<"["<<t->time<<"] "<<t->msg<<endl; t=t->next;}}
};

// Core System Class
class System{
    vector<Car*> fleet; 
    Cust* custHead;
    ServiceQueue sq;
    AuditLog log;
    string adminPass;
public:
    System(){custHead=NULL; loadData(); loadPass();} 

    // CRUD: Create
    void addCar(string i, string m, int y, int h, int s, string t, double r, int rc=0){
        fleet.push_back(new Car(i,m,y,h,s,t,r,rc)); saveData();
    }

    // CRUD: Read & Sort
    void viewShowroom(int sortMode){
        if(sortMode == 1) sort(fleet.begin(), fleet.end(), comparePrice);
        if(sortMode == 2) sort(fleet.begin(), fleet.end(), compareHP);
        cout << CYN << "----------------------------------------------------------------------------------------" << endl;
        cout << "| ID    | Model                           | HP   | Rate          | Stk  | Status      |" << endl;
        cout << "----------------------------------------------------------------------------------------" << RST << endl;
        for(int i=0; i<fleet.size(); i++) fleet[i]->display();
    }

    // CRUD: Search
    Car* findCar(string id){
        for(int i=0; i<fleet.size(); i++) if(toLower(fleet[i]->id) == toLower(id)) return fleet[i];
        return NULL;
    }
    
    void search(string key){
        cout << MAG << "\nSearch Results for '" << key << "':" << RST << endl;
        bool f=0; string lk=toLower(key);
        for(int i=0; i<fleet.size(); i++) if(toLower(fleet[i]->make).find(lk) != string::npos){fleet[i]->display(); f=1;}
        if(!f) cout << RED << "No results." << RST << endl;
        log.push("Searched: " + key);
    }

    // Feature: Rent Logic
    void rentCar(){
        string id; cout << "Car ID: "; cin >> id; Car* c = findCar(id);
        if(c && c->stock > 0){
            int age; cout << "Age: "; cin >> age;
            if(c->hp > 500 && age < 25){cout << RED << "Insurance Denied (High HP)." << RST << endl; return;}
            int days; cout << "Days: "; cin >> days;
            double price = c->getPrice() * days;
            string n, p; cout << "Name: "; cin.ignore(); getline(cin, n); cout << "Phone: "; cin >> p;
            loading("Processing Payment");
            c->stock--; c->rents++; if(c->stock==0) c->status="Rented (Out)";
            Cust* nc = new Cust(n, p, c->make, price, getTimestamp()); nc->next = custHead; custHead = nc;
            cout << GRN << "\n--- RECEIPT ---\nClient:  " << n << " (" << nc->tier << ")\nVehicle: " << c->make << "\nTotal:   $" << price << "\n===============" << RST << endl;
            log.push("Rented " + c->id + " to " + n); saveData();
        } else cout << RED << "Unavailable." << RST << endl;
    }

    void returnCar(){
        string id; cout << "Car ID: "; cin >> id; Car* c = findCar(id);
        if(c && c->stock < c->maxStock){sq.add(c); log.push("Returned " + id); cout << GRN << "Vehicle sent to Service Bay." << RST << endl;}
        else cout << RED << "Invalid Return." << RST << endl;
    }

    // Admin Tools
    void adminPanel(){
        string p; cout << "Admin Password: "; cin >> p;
        if(p != adminPass){cout << RED << "Access Denied." << RST << endl; return;}
        while(true){
            cout << "\n" << MAG << "[ADMIN DASHBOARD]" << RST << "\n1. Add Car\n2. Delete Car\n3. Financial Report\n4. Service Bay\n5. Change Password\n0. Back\nOp: ";
            int op; cin >> op;
            if(op==0) break;
            if(op==1){
                string i,m,t; int y,h,s; double r;
                cout << "ID Model Year HP Stock Trans Rate: "; cin >> i >> m >> y >> h >> s >> t >> r;
                addCar(i,m,y,h,s,t,r); log.push("Admin Added: " + m); cout << GRN << "Added." << RST << endl;
            }
            if(op==2){
                string i; cout << "ID to delete: "; cin >> i;
                for(int k=0; k<fleet.size(); k++){
                    if(fleet[k]->id == i){log.push("Admin Deleted: " + fleet[k]->make); fleet.erase(fleet.begin() + k); cout << YEL << "Deleted." << RST << endl; saveData(); break;}
                }
            }
            if(op==3){
                double total=0; int tx=0; Cust* t=custHead; cout << "\n--- REVENUE STREAM ---" << endl;
                while(t){cout << t->date << " | " << setw(10) << t->name << " | $" << t->spent << " (" << t->tier << ")" << endl; total+=t->spent; tx++; t=t->next;}
                cout << "TOTAL REVENUE: $" << total << " (" << tx << " txns)\nReport exported to Revenue_Report.txt" << endl;
                ofstream rep("Revenue_Report.txt"); rep << "PADDOCK CLUB FINANCIAL REPORT\nGenerated: " << getTimestamp() << "\nTotal Revenue: $" << total << endl; rep.close();
            }
            if(op==4){sq.show(); char c; cout << "Process Next? (y/n): "; cin >> c; if(c=='y') sq.process();}
            if(op==5){cout << "New Password: "; cin >> adminPass; savePass(); cout << GRN << "Password Updated." << RST << endl;}
        }
    }

    // Persistence Logic
    void saveData(){
        ofstream f("db_fleet.csv");
        for(int i=0; i<fleet.size(); i++) f << fleet[i]->id << "," << fleet[i]->make << "," << fleet[i]->year << "," << fleet[i]->hp << "," << fleet[i]->stock << "," << fleet[i]->trans << "," << fleet[i]->baseRate << "," << fleet[i]->rents << endl;
        f.close();
        ofstream fc("db_customers.csv"); Cust* t=custHead;
        while(t){fc << t->name << "," << t->phone << "," << t->carID << "," << t->spent << "," << t->date << endl; t=t->next;}
        fc.close();
    }
    void loadData(){
        ifstream f("db_fleet.csv"); string line;
        while(getline(f, line)){
            stringstream ss(line); string i,m,y,h,s,t,r,rc;
            getline(ss,i,','); getline(ss,m,','); getline(ss,y,','); getline(ss,h,','); getline(ss,s,','); getline(ss,t,','); getline(ss,r,','); getline(ss,rc,',');
            if(i!="") fleet.push_back(new Car(i,m,stoi(y),stoi(h),stoi(s),t,stod(r),stoi(rc)));
        }
        ifstream fc("db_customers.csv");
        while(getline(fc, line)){
            stringstream ss(line); string n,p,ci,s,d;
            getline(ss,n,','); getline(ss,p,','); getline(ss,ci,','); getline(ss,s,','); getline(ss,d,',');
            if(n!=""){Cust* nc=new Cust(n,p,ci,stod(s),d); nc->next=custHead; custHead=nc;}
        }
    }
    void savePass(){ofstream f("admin_pass.txt"); f << adminPass; f.close();}
    void loadPass(){ifstream f("admin_pass.txt"); if(f >> adminPass) f.close(); else adminPass="paddock77";}
    void showLogs(){log.show();}
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
}

int main(){
    System sys;
    if(sys.findCar("MB01") == NULL){ // Pre-seed if database is new
        sys.addCar("MB01","Mercedes C63 Black",2012,510,1,"Auto",1200);
        sys.addCar("FE40","Ferrari F40 Icon",1987,471,1,"Man",8000);
        sys.addCar("TY01","Toyota GR Yaris",2025,280,5,"Man",350);
        sys.addCar("BM01","BMW M5 CS",2022,627,2,"Auto",1800);
    }
    int c;
    while(true){
        system("clear"); header();
        cout << "1. Showroom\n2. Search\n3. Sort (Price/HP)\n4. Rent Vehicle\n5. Return Vehicle\n6. Audit Logs\n7. Admin Panel\n0. Exit\nChoice: "; cin >> c;
        if(c==0) break;
        if(c==1){sys.viewShowroom(0); char b; cout << "\nBook? (y/n): "; cin >> b; if(b=='y') sys.rentCar();}
        if(c==2){string k; cout << "Keyword: "; cin >> k; sys.search(k);}
        if(c==3){int m; cout << "1.Price 2.HP: "; cin >> m; sys.viewShowroom(m);}
        if(c==4) sys.rentCar();
        if(c==5) sys.returnCar();
        if(c==6) sys.showLogs();
        if(c==7) sys.adminPanel();
        cout << "\n(Enter to continue...)"; cin.ignore(); cin.get();
    }
    return 0;
}
