

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <iomanip>
#include <cctype>
#include <ctime>
#include <cstdio>
#include <cstdlib>

using namespace std;

// ============================================================
// STRUCT: Member
// ============================================================
struct Member {
    int id;
    string name;
    int age;
    string gender;        // Male / Female
    string package;       // Basic / Standard / Premium
    string joinDate;
    string phone;
    bool isActive;

    Member() {}
    Member(int id, string name, int age, string gender,
           string package, string joinDate, string phone)
        : id(id), name(name), age(age), gender(gender),
          package(package), joinDate(joinDate), phone(phone), isActive(true) {}
};

// ============================================================
// GLOBAL DATA STRUCTURES
// ============================================================
vector<Member> members;                        // Main list
unordered_map<int, int> hashTable;             // id -> index in members vector
int nextId = 1001;                             // Auto-increment ID

// ============================================================
// UTILITY FUNCTIONS
// ============================================================

string getCurrentDate() {
    time_t t = time(0);
    tm* now = localtime(&t);
    char buf[20];
    sprintf(buf, "%02d/%02d/%04d",
            now->tm_mday, now->tm_mon + 1, now->tm_year + 1900);
    return string(buf);
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void printLine(char c = '-', int len = 60) {
    cout << string(len, c) << endl;
}

void printHeader(const string& title) {
    printLine('=');
    cout << setw(35 + title.size() / 2) << title << endl;
    printLine('=');
}

string toLower(string s) {
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

void printMember(const Member& m) {
    printLine();
    cout << "  ID       : " << m.id << endl;
    cout << "  Name     : " << m.name << endl;
    cout << "  Age      : " << m.age << endl;
    cout << "  Gender   : " << m.gender << endl;
    cout << "  Package  : " << m.package << endl;
    cout << "  Phone    : " << m.phone << endl;
    cout << "  Joined   : " << m.joinDate << endl;
    cout << "  Status   : " << (m.isActive ? "Active" : "Inactive") << endl;
    printLine();
}

void printTable(const vector<Member>& list) {
    if (list.empty()) {
        cout << "  No members found.\n";
        return;
    }
    printLine('-', 80);
    cout << left
         << setw(7)  << "ID"
         << setw(22) << "Name"
         << setw(6)  << "Age"
         << setw(10) << "Gender"
         << setw(12) << "Package"
         << setw(13) << "Phone"
         << setw(10) << "Status"
         << endl;
    printLine('-', 80);
    for (const auto& m : list) {
        cout << left
             << setw(7)  << m.id
             << setw(22) << m.name
             << setw(6)  << m.age
             << setw(10) << m.gender
             << setw(12) << m.package
             << setw(13) << m.phone
             << setw(10) << (m.isActive ? "Active" : "Inactive")
             << endl;
    }
    printLine('-', 80);
    cout << "  Total: " << list.size() << " member(s)\n";
}

// Rebuild hash table after any sort/modification
void rebuildHashTable() {
    hashTable.clear();
    for (int i = 0; i < (int)members.size(); i++) {
        hashTable[members[i].id] = i;
    }
}

// ============================================================
// ALGORITHM 1: MERGE SORT (by name or by ID)
// ============================================================

void merge(vector<Member>& arr, int l, int m, int r, bool byName) {
    int n1 = m - l + 1, n2 = r - m;
    vector<Member> L(arr.begin() + l, arr.begin() + l + n1);
    vector<Member> R(arr.begin() + m + 1, arr.begin() + m + 1 + n2);

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        bool cond = byName ? (toLower(L[i].name) <= toLower(R[j].name))
                           : (L[i].id <= R[j].id);
        if (cond) arr[k++] = L[i++];
        else       arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

void mergeSort(vector<Member>& arr, int l, int r, bool byName) {
    if (l >= r) return;
    int m = l + (r - l) / 2;
    mergeSort(arr, l, m, byName);
    mergeSort(arr, m + 1, r, byName);
    merge(arr, l, m, r, byName);
}

// ============================================================
// ALGORITHM 2: BINARY SEARCH (by ID — requires sorted list)
// ============================================================

int binarySearchById(const vector<Member>& arr, int targetId) {
    int low = 0, high = (int)arr.size() - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (arr[mid].id == targetId)       return mid;
        else if (arr[mid].id < targetId)   low = mid + 1;
        else                               high = mid - 1;
    }
    return -1;
}

// ============================================================
// ALGORITHM 3: LINEAR SEARCH (by name — partial match)
// ============================================================

vector<Member> linearSearchByName(const vector<Member>& arr, const string& query) {
    vector<Member> results;
    string q = toLower(query);
    for (const auto& m : arr) {
        if (toLower(m.name).find(q) != string::npos) {
            results.push_back(m);
        }
    }
    return results;
}

// ============================================================
// ALGORITHM 4: HASH TABLE LOOKUP (O(1) by ID)
// ============================================================

Member* hashLookup(int id) {
    auto it = hashTable.find(id);
    if (it != hashTable.end()) {
        return &members[it->second];
    }
    return nullptr;
}

// ============================================================
// FEATURE: ADD MEMBER
// ============================================================

void addMember() {
    printHeader("ADD NEW MEMBER");

    Member m;
    m.id = nextId++;

    cout << "  Name     : "; cin.ignore(); getline(cin, m.name);
    cout << "  Age      : "; cin >> m.age;
    cin.ignore();
    cout << "  Gender (Male/Female): "; getline(cin, m.gender);
    cout << "  Phone    : "; getline(cin, m.phone);

    cout << "\n  Select Package:\n";
    cout << "  [1] Basic    - Rs. 2,000/month\n";
    cout << "  [2] Standard - Rs. 3,500/month\n";
    cout << "  [3] Premium  - Rs. 5,000/month\n";
    cout << "  Choice: ";
    int ch; cin >> ch;
    if      (ch == 1) m.package = "Basic";
    else if (ch == 2) m.package = "Standard";
    else              m.package = "Premium";

    m.joinDate = getCurrentDate();
    m.isActive = true;

    members.push_back(m);
    hashTable[m.id] = (int)members.size() - 1;

    cout << "\n  [SUCCESS] Member registered! Assigned ID: " << m.id << endl;
    cout << "  Press Enter to continue..."; cin.ignore(); cin.get();
}

// ============================================================
// FEATURE: DISPLAY ALL MEMBERS
// ============================================================

void displayAllMembers() {
    printHeader("ALL MEMBERS");
    if (members.empty()) {
        cout << "  No members registered yet.\n";
    } else {
        printTable(members);
    }
    cout << "\n  Press Enter to continue...";
    cin.ignore(); cin.get();
}

// ============================================================
// FEATURE: SEARCH MEMBER
// ============================================================

void searchMember() {
    printHeader("SEARCH MEMBER");
    cout << "  [1] Search by ID   (Binary Search - O(log n))\n";
    cout << "  [2] Search by Name (Linear Search - O(n))\n";
    cout << "  [3] Quick Lookup   (Hash Table    - O(1))\n";
    cout << "  Choice: ";
    int ch; cin >> ch;

    if (ch == 1) {
        // Sort by ID first, then binary search
        vector<Member> sorted = members;
        mergeSort(sorted, 0, (int)sorted.size() - 1, false);

        int id;
        cout << "  Enter Member ID: "; cin >> id;
        int idx = binarySearchById(sorted, id);
        if (idx == -1) cout << "\n  [NOT FOUND] No member with ID " << id << endl;
        else           printMember(sorted[idx]);

    } else if (ch == 2) {
        string name;
        cout << "  Enter Name (or partial): "; cin.ignore(); getline(cin, name);
        vector<Member> results = linearSearchByName(members, name);
        if (results.empty()) cout << "\n  [NOT FOUND] No matching member.\n";
        else                 printTable(results);

    } else if (ch == 3) {
        int id;
        cout << "  Enter Member ID: "; cin >> id;
        Member* m = hashLookup(id);
        if (!m) cout << "\n  [NOT FOUND] No member with ID " << id << endl;
        else    printMember(*m);
    }

    cout << "\n  Press Enter to continue..."; cin.ignore(); cin.get();
}

// ============================================================
// FEATURE: SORT MEMBERS
// ============================================================

void sortMembers() {
    printHeader("SORT MEMBERS");
    cout << "  [1] Sort by Name (A-Z)\n";
    cout << "  [2] Sort by ID   (Ascending)\n";
    cout << "  Choice: ";
    int ch; cin >> ch;

    bool byName = (ch == 1);
    mergeSort(members, 0, (int)members.size() - 1, byName);
    rebuildHashTable();

    cout << "\n  [SUCCESS] Members sorted using Merge Sort!\n\n";
    printTable(members);
    cout << "\n  Press Enter to continue..."; cin.ignore(); cin.get();
}

// ============================================================
// FEATURE: UPDATE MEMBER
// ============================================================

void updateMember() {
    printHeader("UPDATE MEMBER");
    int id;
    cout << "  Enter Member ID to update: "; cin >> id;

    Member* m = hashLookup(id);
    if (!m) {
        cout << "\n  [NOT FOUND] No member with ID " << id << endl;
        cout << "  Press Enter to continue..."; cin.ignore(); cin.get();
        return;
    }

    printMember(*m);
    cout << "\n  What to update?\n";
    cout << "  [1] Phone\n  [2] Package\n  [3] Status (Active/Inactive)\n";
    cout << "  Choice: ";
    int ch; cin >> ch; cin.ignore();

    if (ch == 1) {
        cout << "  New Phone: "; getline(cin, m->phone);
    } else if (ch == 2) {
        cout << "  [1] Basic  [2] Standard  [3] Premium\n  Choice: ";
        int p; cin >> p;
        if      (p == 1) m->package = "Basic";
        else if (p == 2) m->package = "Standard";
        else             m->package = "Premium";
    } else if (ch == 3) {
        m->isActive = !m->isActive;
        cout << "  Status toggled to: " << (m->isActive ? "Active" : "Inactive") << endl;
    }

    cout << "\n  [SUCCESS] Member updated.\n";
    cout << "  Press Enter to continue..."; cin.ignore(); cin.get();
}

// ============================================================
// FEATURE: DELETE MEMBER
// ============================================================

void deleteMember() {
    printHeader("DELETE MEMBER");
    int id;
    cout << "  Enter Member ID to delete: "; cin >> id;

    auto it = hashTable.find(id);
    if (it == hashTable.end()) {
        cout << "\n  [NOT FOUND] No member with ID " << id << endl;
        cout << "  Press Enter to continue..."; cin.ignore(); cin.get();
        return;
    }

    int idx = it->second;
    cout << "\n  Deleting: " << members[idx].name << " (ID: " << id << ")\n";
    cout << "  Confirm? (y/n): ";
    char c; cin >> c;
    if (c == 'y' || c == 'Y') {
        members.erase(members.begin() + idx);
        rebuildHashTable();
        cout << "  [SUCCESS] Member deleted.\n";
    } else {
        cout << "  Cancelled.\n";
    }

    cout << "  Press Enter to continue..."; cin.ignore(); cin.get();
}

// ============================================================
// FEATURE: STATISTICS
// ============================================================

void showStats() {
    printHeader("GYM STATISTICS");

    int active = 0, inactive = 0;
    int basic = 0, standard = 0, premium = 0;
    int male = 0, female = 0;

    for (const auto& m : members) {
        if (m.isActive) active++; else inactive++;
        if (m.package == "Basic")    basic++;
        else if (m.package == "Standard") standard++;
        else premium++;
        if (m.gender == "Male" || m.gender == "male") male++;
        else female++;
    }

    int total = (int)members.size();
    int revenue = basic * 2000 + standard * 3500 + premium * 5000;

    cout << "\n  MEMBERSHIP OVERVIEW\n";
    printLine();
    cout << "  Total Members   : " << total    << endl;
    cout << "  Active          : " << active   << endl;
    cout << "  Inactive        : " << inactive << endl;
    cout << "\n  PACKAGES\n";
    printLine();
    cout << "  Basic           : " << basic    << "  (Rs. " << basic * 2000    << "/month)\n";
    cout << "  Standard        : " << standard << "  (Rs. " << standard * 3500 << "/month)\n";
    cout << "  Premium         : " << premium  << "  (Rs. " << premium * 5000  << "/month)\n";
    cout << "\n  GENDER SPLIT\n";
    printLine();
    cout << "  Male            : " << male   << endl;
    cout << "  Female          : " << female << endl;
    cout << "\n  ESTIMATED MONTHLY REVENUE: Rs. " << revenue << endl;
    printLine();

    cout << "\n  Press Enter to continue..."; cin.ignore(); cin.get();
}

// ============================================================
// LOAD SAMPLE DATA
// ============================================================

void loadSampleData() {
    vector<Member> samples = {
        {nextId++, "Ali Hassan",     22, "Male",   "Premium",  "01/01/2025", "0300-1111111"},
        {nextId++, "Sara Khan",      28, "Female", "Standard", "15/02/2025", "0301-2222222"},
        {nextId++, "Usman Tariq",    35, "Male",   "Basic",    "10/03/2025", "0302-3333333"},
        {nextId++, "Ayesha Malik",   24, "Female", "Premium",  "05/04/2025", "0303-4444444"},
        {nextId++, "Bilal Ahmed",    30, "Male",   "Standard", "20/01/2025", "0304-5555555"},
        {nextId++, "Zara Noor",      26, "Female", "Basic",    "11/02/2025", "0305-6666666"},
        {nextId++, "Hamza Sheikh",   19, "Male",   "Basic",    "03/05/2025", "0306-7777777"},
        {nextId++, "Nadia Iqbal",    31, "Female", "Standard", "22/03/2025", "0307-8888888"},
    };
    for (auto& m : samples) {
        members.push_back(m);
        hashTable[m.id] = (int)members.size() - 1;
    }
}

// ============================================================
// MAIN MENU
// ============================================================

int main() {
    loadSampleData();

    while (true) {
        clearScreen();
        printHeader("GYM MANAGEMENT SYSTEM");
        cout << "\n";
        cout << "  [1]  Add New Member\n";
        cout << "  [2]  Display All Members\n";
        cout << "  [3]  Search Member\n";
        cout << "  [4]  Sort Members (Merge Sort)\n";
        cout << "  [5]  Update Member\n";
        cout << "  [6]  Delete Member\n";
        cout << "  [7]  Statistics & Revenue\n";
        cout << "  [0]  Exit\n";
        cout << "\n";
        printLine();
        cout << "  Choice: ";

        int choice; cin >> choice;

        switch (choice) {
            case 1: clearScreen(); addMember();       break;
            case 2: clearScreen(); displayAllMembers(); break;
            case 3: clearScreen(); searchMember();    break;
            case 4: clearScreen(); sortMembers();     break;
            case 5: clearScreen(); updateMember();    break;
            case 6: clearScreen(); deleteMember();    break;
            case 7: clearScreen(); showStats();       break;
            case 0:
                cout << "\n  Exiting... Thank you!\n\n";
                return 0;
            default:
                cout << "\n  Invalid choice. Try again.\n";
                cin.ignore(); cin.get();
        }
    }

    return 0;
}