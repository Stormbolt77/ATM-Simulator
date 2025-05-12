﻿#include <iostream>  // Επιτρέπει την είσοδο και έξοδο δεδομένων από και προς την κονσόλα
#include <fstream>   // Παρέχει λειτουργίες για ανάγνωση και εγγραφή αρχείων
#include <sstream>   // Χρησιμοποιείται για επεξεργασία string σαν να ήταν ροές δεδομένων (π.χ. istringstream, ostringstream)
#include <string>    // Περιέχει τον τύπο string και συναρτήσεις χειρισμού string
#include <vector>    // Παρέχει τον δυναμικό πίνακα vector και τις συναρτήσεις του
#include <cstdlib>   // Περιλαμβάνει συναρτήσεις της C όπως rand(), system(), exit() για βασικές λειτουργίες
#include <chrono>    // Παρέχει εργαλεία για μέτρηση και διαχείριση του χρόνου
#include <thread>    // Επιτρέπει την χρήση νημάτων (threads) και καθυστερήσεων, π.χ. this_thread::sleep_for()
#include <iomanip>   // Παρέχει ρυθμίσεις μορφοποίησης εξόδου, όπως αριθμητική ακρίβεια (π.χ. setprecision())
using namespace std;

// Αυτή η συνάρτηση καθαρίζει την οθόνη της κονσόλας ανάλογα με το λειτουργικό σύστημα
void clearScreen() {
#ifdef _WIN32
    system("cls");  // For Windows
#else
    system("clear");  // For Linux/Mac
#endif
}

// Εμφανίζει μια οθόνη αναμονής με τελείες που καθυστερεί για 3 δευτερόλεπτα, δημιουργώντας αίσθηση φόρτωσης ή επεξεργασίας
void loadingScreen(int delaySeconds = 2) {
    cout << "Please wait";
    for (int i = 0; i < 3; i++) {
        cout << ".";
        cout.flush();
        this_thread::sleep_for(chrono::seconds(1));  // Delay για 1 second
    }
    cout << "\n";
}

// Ελέγχει αν τα στοιχεία που εισάγει ο χρήστης, όνομα και PIN, υπάρχουν στο αρχείο users.txt
bool login(string& username, double& balance) {
    string enteredUser, enteredPin, storedUser, storedPin;
    double storedBalance;
    bool found = false;

    cout << "ATM Login\n";
    cout << "Username: ";
    cin >> enteredUser;
    cout << "PIN: ";
    cin >> enteredPin;

    ifstream usersFile("users.txt");

    while (getline(usersFile, storedUser, ',')) {
        getline(usersFile, storedPin, ',');
        usersFile >> storedBalance;
        usersFile.ignore();

        if (enteredUser == storedUser && enteredPin == storedPin) {
            username = storedUser;
            balance = storedBalance;
            found = true;
            break;
        }
    }
    usersFile.close();

    if (!found) {
        cout << "Error: Wrong Username or PIN!\n";
        return false;
    }

    cout << "Login successful! Welcome, " << username << "!\n";
    cout << fixed << setprecision(2);
    cout << "Your current balance is: $" << balance << endl;
    return true;
}

// Προσθέτει νέο χρήστη στο αρχείο users.txt με αρχικό υπόλοιπο $2000
void registerUser() {
    string newUsername, newPassword;
    double startingBalance = 2000.00;

    cout << "Register New User\n";
    cout << "Enter a username: ";
    cin >> newUsername;
    cout << "Enter a PIN: ";
    cin >> newPassword;

    // Εδώ ελέγχει για υπάρχον όνομα χρήστη
    ifstream usersFile("users.txt");
    string storedUser, storedPin;
    double storedBalance;
    bool usernameExists = false;

    while (getline(usersFile, storedUser, ',')) {
        getline(usersFile, storedPin, ',');
        usersFile >> storedBalance;
        usersFile.ignore();

        if (storedUser == newUsername) {
            usernameExists = true;
            break;
        }
    }
    usersFile.close();

    if (usernameExists) {
        cout << "Error: Username already exists!\n";
        return;
    }

    // Προσθήκη νέου χρήστη στο αρχείο users.txt
    ofstream usersFileOut("users.txt", ios::app);
    usersFileOut << newUsername << "," << newPassword << "," << fixed << setprecision(2) << startingBalance << endl;
    usersFileOut.close();

    cout << "Registration successful! Your starting balance is: $" << startingBalance << endl;
}

// Λειτουργία ανάληψης χρημάτων από λογαριασμό χρήστη
void withdrawMoney(string username, double& balance) {
    loadingScreen();  // Show loading screen before operation

    double amount;
    cout << "Please enter amount to withdraw: ";
    cin >> amount;

    if (amount <= 0) {
        cout << "Invalid input! Please enter a valid amount.\n";
        return;
    }

    if (amount > balance) {
        cout << "Insufficient funds!\n";
        return;
    }

    loadingScreen();

    // Αφαίρεση του ποσού από το υπόλοιπο
    balance -= amount;
    cout << fixed << setprecision(2);  // Ensure 2 decimal places
    cout << "Transaction Complete! Your new balance is: $" << balance << endl;

    // Ενημέρωση users.txt file
    ifstream usersFile("users.txt");
    ofstream tempFile("temp.txt");

    string line, fileUser, filePIN;
    double fileBalance;

    while (getline(usersFile, line)) {
        stringstream ss(line);
        getline(ss, fileUser, ',');
        getline(ss, filePIN, ',');
        ss >> fileBalance;

        if (fileUser == username) {
            tempFile << fileUser << "," << filePIN << "," << fixed << setprecision(2) << balance << endl;
        }
        else {
            tempFile << line << endl;
        }
    }

    usersFile.close();
    tempFile.close();

    remove("users.txt");
    rename("temp.txt", "users.txt");

    // Καταγραφή της συναλλαγής στο transaksions.txt
    ofstream transactionsFile("transactions.txt", ios::app);
    transactionsFile << username << ",Withdrawal,-" << fixed << setprecision(2) << amount << endl;
    transactionsFile.close();
}

// Yλοποιεί την κατάθεση χρημάτων σε έναν λογαριασμό χρήστη
void depositMoney(string username, double& balance) {
    loadingScreen();

    double amount;
    cout << "Enter amount to deposit: ";
    cin >> amount;

    if (amount <= 0) {
        cout << "Invalid input! Please enter a valid amount\n";
        return;
    }

    loadingScreen();

    balance += amount;
    cout << fixed << setprecision(2);
    cout << "Deposit completed! Your new balance is: $" << balance << endl;


    ifstream usersFile("users.txt");
    ofstream tempFile("temp.txt");

    string line, fileUser, filePIN;
    double fileBalance;

    while (getline(usersFile, line)) {
        stringstream ss(line);
        getline(ss, fileUser, ',');
        getline(ss, filePIN, ',');
        ss >> fileBalance;

        if (fileUser == username) {
            tempFile << fileUser << "," << filePIN << "," << fixed << setprecision(2) << balance << endl;
        }
        else {
            tempFile << line << endl;
        }
    }

    usersFile.close();
    tempFile.close();

    remove("users.txt");
    rename("temp.txt", "users.txt");


    ofstream transactionsFile("transactions.txt", ios::app);
    transactionsFile << username << ",Deposit,+" << fixed << setprecision(2) << amount << endl;
    transactionsFile.close();
}

// Δείχνει στον χρήστη το ιστορικό συναλλαγών του λογαριασμού του
void viewTransactions(string username) {
    loadingScreen();

    ifstream transactionsFile("transactions.txt");

    if (!transactionsFile) {
        cout << "Error: No transaction file found!\n";
        return;
    }

    string line, fileUser, type, detail, amount;
    bool hasTransactions = false;

    cout << "\n--- Transactions History ---\n";

    while (getline(transactionsFile, line)) {
        stringstream ss(line);
        getline(ss, fileUser, ',');
        getline(ss, type, ',');
        getline(ss, detail, ',');
        getline(ss, amount, ',');

        if (fileUser == username) {
            if (type == "TransferTo" || type == "TransferFrom") {

                cout << type << " " << detail << ": $" << fixed << setprecision(2) << amount << endl;
            }
            else if (type == "Deposit" || type == "Withdrawal") {

                cout << type << ": $" << fixed << setprecision(2) << amount << endl;
            }
            hasTransactions = true;
        }
    }

    if (!hasTransactions) {
        cout << "There are no recent transactions for user: " << username << ".\n";
    }

    transactionsFile.close();
}

// Yπολογίζει τον φόρο βάσει του ετήσιου εισοδήματος του χρήστη
void calculateTax() {
    loadingScreen();

    double salary, tax = 0;
    cout << "Please enter your annual salary ($): ";
    cin >> salary;

    loadingScreen();

    if (salary <= 19500) {
        tax = 0;
    }
    else if (salary <= 28000) {
        tax = (salary - 19500) * 0.20;
    }
    else if (salary <= 36300) {
        tax = (28000 - 19500) * 0.20 + (salary - 28000) * 0.25;
    }
    else if (salary <= 60000) {
        tax = (28000 - 19500) * 0.20 + (36300 - 28000) * 0.25 + (salary - 36300) * 0.30;
    }
    else {
        tax = (28000 - 19500) * 0.20 + (36300 - 28000) * 0.25 + (60000 - 36300) * 0.30 + (salary - 60000) * 0.35;
    }

    double netIncome = salary - tax;

    cout << fixed << setprecision(2);
    cout << "Your taxation will be: $" << tax << endl;
    cout << "Your Net salary after tax: $" << netIncome << endl;
}

// Χρησιμοποιείται για να μεταφέρει χρήματα από τον χρήστη σε άλλο χρήστη
void transferMoney(string username, double& balance) {
    loadingScreen();

    string recipient;
    double amount;
    bool recipientExists = false;
    vector<string> users;
    vector<string> pins;
    vector<double> balances;

    cout << "Enter the name of the recipient: ";
    cin >> recipient;

    if (recipient == username) {
        cout << "You cannot send money to your own account!\n";
        return;
    }

    cout << "Enter the amount to send: ";
    cin >> amount;

    if (amount <= 0) {
        cout << "Invalid amount!\n";
        return;
    }

    if (amount > balance) {
        cout << "Error: Insufficient funds!\n";
        return;
    }

    loadingScreen();

    ifstream usersFile("users.txt");
    string user, pin;
    double userBalance;

    while (getline(usersFile, user, ',')) {
        getline(usersFile, pin, ',');
        usersFile >> userBalance;
        usersFile.ignore();

        users.push_back(user);
        pins.push_back(pin);
        balances.push_back(userBalance);

        if (user == recipient) {
            recipientExists = true;
        }
    }
    usersFile.close();

    if (!recipientExists) {
        cout << "Error: User '" << recipient << "' could not be found!\n";
        return;
    }


    for (size_t i = 0; i < users.size(); i++) {
        if (users[i] == username) {
            balances[i] -= amount;
            balance = balances[i];
        }
        if (users[i] == recipient) {
            balances[i] += amount;
        }
    }


    ofstream usersOut("users.txt");
    for (size_t i = 0; i < users.size(); i++) {
        usersOut << users[i] << "," << pins[i] << "," << fixed << setprecision(2) << balances[i] << endl;
    }
    usersOut.close();


    ofstream transactionsFile("transactions.txt", ios::app);
    transactionsFile << username << ",TransferTo," << recipient << "," << fixed << setprecision(2) << -amount << endl;
    transactionsFile << recipient << ",TransferFrom," << username << "," << fixed << setprecision(2) << amount << endl;
    transactionsFile.close();

    cout << fixed << setprecision(2);
    cout << "The transaction of $" << amount << " to user " << recipient << " has been completed!\n";
    cout << "Your updated balance is: $" << balance << endl;
}

int main() {
    string username;
    double balance;

    while (true) {  // loop
        clearScreen();

        cout << "Welcome to the ATM Simulator\n";
        cout << "1. Login\n";
        cout << "2. Register\n";
        cout << "3. Exit\n";
        int choice;
        cin >> choice;

        switch (choice) {
        case 1:  // Login
            if (!login(username, balance)) {
                continue; // Εάν η σύνδεση αποτύχει, δοκιμάζει ξανά
            }
            break;

        case 2:  // Register
            registerUser();
            continue;  // Επιστροφή στο κύριο μενού μετά την εγγραφή

        case 3:  // Exit
            cout << "Exiting...\n";
            return 0;

        default:
            cout << "Not a valid response!\n";
            continue;
        }

        // Κυρίος μενού μετά από επιτυχή σύνδεση
        while (true) {  // Menu loop
            cout << "\nPlease choose an option: \n";
            cout << "1. Withdraw\n";
            cout << "2. Deposit\n";
            cout << "3. Transfer Money\n";
            cout << "4. Transactions History\n";
            cout << "5. Annual Salary TAX Calculator\n";
            cout << "6. Logout\n";
            cin >> choice;

            clearScreen();

            switch (choice) {
            case 1:
                withdrawMoney(username, balance);
                break;

            case 2:
                depositMoney(username, balance);
                break;

            case 3:
                transferMoney(username, balance);
                break;

            case 4:
                viewTransactions(username);
                break;

            case 5:
                calculateTax();
                break;

            case 6:
                cout << "Logging out...\n";
                return 0;

            default:
                cout << "Not a valid response!\n";
            }

            // Εμφάνιση ενημερωμένου υπολοίπου μετά από κάθε ενέργεια
            cout << fixed << setprecision(2);
            cout << "Your current balance is: $" << balance << endl;
        }
    }

    return 0;
}