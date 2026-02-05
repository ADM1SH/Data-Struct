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

// Utils
string getTimestamp(){time_t now=time(0); char* dt=ctime(&now); string s(dt); return s.substr(0, s.length()-1);}
string toLower(string s){for(int i=0; i<s.length(); i++) s[i]=tolower(s[i]); return s;}
void loading(string msg){cout << YEL << msg << flush; for(int i=0;i<3;i++){this_thread::sleep_for(chrono::milliseconds(200)); cout << "." << flush;} cout << RST << endl;}

// Data Structure: Car Object (Updated with Top Speed 'ts')
struct Car{
    string id, make, trans, status;
    int year, hp, ts, stock, maxStock, rents;
    double baseRate;
    Car(string i, string m, int y, int h, int t_spd, string t, double r, int s, int rc=0){
        id=i; make=m; year=y; hp=h; ts=t_spd; trans=t; baseRate=r; stock=s; maxStock=s; rents=rc;
        status=(s>0)?"Available":"Out of Stock";
    }
    double getPrice(){return (stock == 1 && maxStock > 1) ? baseRate * 1.2 : baseRate;} 
    void display() const {
        double currRate = (stock == 1 && maxStock > 1) ? baseRate * 1.2 : baseRate;
        string rateStr = "$" + to_string((int)currRate) + ((currRate > baseRate) ? "!" : "");
        cout << "| " << left << setw(6) << id << "| " << setw(32) << make << "| " << setw(5) << hp << "| " << setw(5) << ts << "| " << setw(10) << rateStr << "| " << setw(5) << stock << "| " << setw(12) << status << "|" << endl;
    }
};

bool comparePrice(Car* a, Car* b){return a->getPrice() > b->getPrice();}
bool compareHP(Car* a, Car* b){return a->hp > b->hp;}

struct Cust{
    string name, phone, carID, tier, date;
    double spent; Cust* next;
    Cust(string n, string p, string c, double s, string d){
        name=n; phone=p; carID=c; spent=s; date=d; next=NULL;
        tier = (spent > 5000) ? "PLATINUM" : (spent > 2000) ? "GOLD" : "STANDARD";
    }
};

class ServiceQueue{
    Car* q[10]; int f, r, sz;
public:
    ServiceQueue(){f=0; r=-1; sz=0;}
    void add(Car* c){if(sz==10){cout<<RED<<"Bay Full!"<<RST<<endl;return;} r=(r+1)%10; q[r]=c; sz++; c->status="Servicing";}
    void process(){if(sz==0)return; Car* c=q[f]; f=(f+1)%10; sz--; if(c->stock<c->maxStock) c->stock++; c->status=(c->stock>0)?"Available":"Out"; cout<<GRN<<"Ready: "<<c->make<<RST<<endl;}
    void show(){if(sz==0)cout<<YEL<<"[Empty]"<<RST<<endl; else for(int i=0,j=f; i<sz; i++,j=(j+1)%10) cout<<i+1<<". "<<q[j]->make<<endl;}
};

struct Log{string msg, time; Log* next; Log(string m){msg=m; time=getTimestamp(); next=NULL;}};
class AuditLog{
    Log* top;
public:
    AuditLog(){top=NULL;}
    void push(string m){
        Log* n=new Log(m); n->next=top; top=n;
        ofstream f("audit_log.txt", ios::app); f << "[" << n->time << "] " << m << endl; f.close();
    }
    void show(){Log* t=top; if(!t) cout<<YEL<<"Empty."<<RST<<endl; while(t){cout<<"["<<t->time<<"] "<<t->msg<<endl; t=t->next;}}
};

class System{
    vector<Car*> fleet; Cust* custHead; ServiceQueue sq; AuditLog log; string adminPass;
public:
    System(){custHead=NULL; loadData(); loadPass();} 
    void addCar(string i, string m, int y, int h, int ts, string t, double r, int s, int rc=0){
        fleet.push_back(new Car(i,m,y,h,ts,t,r,s,rc)); saveData();
    }
    void viewShowroom(int sortMode){
        if(sortMode == 1) sort(fleet.begin(), fleet.end(), comparePrice);
        if(sortMode == 2) sort(fleet.begin(), fleet.end(), compareHP);
        cout << CYN << "------------------------------------------------------------------------------------------------" << endl;
        cout << "| ID    | Model                           | HP   | KPH  | Rate      | Stk  | Status      |" << endl;
        cout << "------------------------------------------------------------------------------------------------" << RST << endl;
        for(int i=0; i<fleet.size(); i++) fleet[i]->display();
    }
    Car* findCar(string id){
        for(int i=0; i<fleet.size(); i++) if(toLower(fleet[i]->id) == toLower(id)) return fleet[i];
        return NULL;
    }
    void search(string key){
        cout << MAG << "\nSearch: " << key << RST << endl;
        bool f=0; string lk=toLower(key);
        for(int i=0; i<fleet.size(); i++) if(toLower(fleet[i]->make).find(lk) != string::npos){fleet[i]->display(); f=1;}
        if(!f) cout << RED << "No results." << RST << endl;
        log.push("Searched: " + key);
    }
    void rentCar(){
        string id; cout << "Car ID: "; cin >> id; Car* c = findCar(id);
        if(c && c->stock > 0){
            int age; cout << "Age: "; cin >> age;
            if(c->hp > 500 && age < 25){cout << RED << "Insurance Denied." << RST << endl; return;}
            int days; cout << "Days: "; cin >> days;
            double price = c->getPrice() * days;
            string n, p; cout << "Name: "; cin.ignore(); getline(cin, n); cout << "Phone: "; cin >> p;
            loading("Payment");
            c->stock--; c->rents++; if(c->stock==0) c->status="Rented (Out)";
            Cust* nc = new Cust(n, p, c->make, price, getTimestamp()); nc->next = custHead; custHead = nc;
            cout << GRN << "\n--- RECEIPT ---\nClient:  " << n << " (" << nc->tier << ")\nVehicle: " << c->make << "\nTotal:   $" << price << "\n===============" << RST << endl;
            log.push("Rented " + c->id + " to " + n); saveData();
        } else cout << RED << "Unavailable." << RST << endl;
    }
    void returnCar(){
        string id; cout << "Car ID: "; cin >> id; Car* c = findCar(id);
        if(c && c->stock < c->maxStock){sq.add(c); log.push("Returned " + id); cout << GRN << "Sent to Wash Bay." << RST << endl;}
        else cout << RED << "Invalid Return." << RST << endl;
    }
    void adminPanel(){
        string p; cout << "Admin Pass: "; cin >> p;
        if(p != adminPass){cout << RED << "Denied." << RST << endl; return;}
        while(true){
            cout << "\n" << MAG << "[ADMIN]" << RST << "\n1. Add\n2. Delete\n3. Financials\n4. Wash Bay\n5. Password\n0. Back\nOp: ";
            int op; cin >> op; if(op==0) break;
            if(op==1){
                string i,m,t; int y,h,ts,s; double r;
                cout << "ID Model Year HP TS Stock Trans Rate: "; cin >> i; cin.ignore(); getline(cin, m); cin >> y >> h >> ts >> s >> t >> r;
                addCar(i,m,y,h,ts,t,r,s); cout << GRN << "Added." << RST << endl;
            }
            if(op==2){
                string i; cout << "ID: "; cin >> i;
                for(int k=0; k<fleet.size(); k++) if(fleet[k]->id == i){log.push("Admin Deleted: " + fleet[k]->make); fleet.erase(fleet.begin() + k); cout << YEL << "Deleted." << RST << endl; saveData(); break;}
            }
            if(op==3){
                double total=0; Cust* t=custHead; cout << "\n--- REVENUE ---" << endl;
                while(t){cout << t->date << " | " << setw(10) << t->name << " | $" << t->spent << " (" << t->tier << ")" << endl; total+=t->spent; t=t->next;}
                cout << "TOTAL: $" << total << endl;
            }
            if(op==4){sq.show(); char c; cout << "Next? (y/n): "; cin >> c; if(c=='y') sq.process();}
            if(op==5){cout << "New Pass: "; cin >> adminPass; savePass(); cout << GRN << "Saved." << RST << endl;}
        }
    }
    void saveData(){
        ofstream f("db_fleet.csv");
        for(int i=0; i<fleet.size(); i++) f << fleet[i]->id << "," << fleet[i]->make << "," << fleet[i]->year << "," << fleet[i]->hp << "," << fleet[i]->ts << "," << fleet[i]->trans << "," << fleet[i]->baseRate << "," << fleet[i]->stock << "," << fleet[i]->rents << endl;
        f.close();
        ofstream fc("db_customers.csv"); Cust* t=custHead;
        while(t){fc << t->name << "," << t->phone << "," << t->carID << "," << t->spent << "," << t->date << endl; t=t->next;}
        fc.close();
    }
    void loadData(){
        ifstream f("db_fleet.csv"); string line;
        while(getline(f, line)){
            if(line.empty()) continue;
            stringstream ss(line); string i,m,y,h,ts,tr,r,s,rc;
            getline(ss,i,','); getline(ss,m,','); getline(ss,y,','); getline(ss,h,','); getline(ss,ts,','); getline(ss,tr,','); getline(ss,r,','); getline(ss,s,','); getline(ss,rc,',');
            if(!i.empty()) fleet.push_back(new Car(i,m,stoi(y),stoi(h),stoi(ts),tr,stod(r),stoi(s),stoi(rc)));
        }
        ifstream fc("db_customers.csv");
        while(getline(fc, line)){
            if(line.empty()) continue;
            stringstream ss(line); string n,p,ci,s,d;
            getline(ss,n,','); getline(ss,p,','); getline(ss,ci,','); getline(ss,s,','); getline(ss,d,',');
            if(!n.empty()){Cust* nc=new Cust(n,p,ci,stod(s),d); nc->next=custHead; custHead=nc;}
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