#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cctype>
using namespace std;

enum AccountType { SAVINGS, CURRENT };

class BankAccount {
private:
    int accountNumber;
    string name;
    string password;
    int pin;
    string securityAnswer;
    string aadhaar;
    string pan;
    AccountType type;
    double balance;

    int attempts = 0;
    bool isLocked = false;
    time_t lockTime;

    int withdrawalCount = 0;
    vector<string> transactions;

public:
    // VALIDATIONS 
    bool isValidPassword(string pass) {
        if (pass.length() < 8) return false;
        bool upper = false, lower = false, digit = false, special = false;
        for (char c : pass) {
            if (isupper(c)) upper = true;
            else if (islower(c)) lower = true;
            else if (isdigit(c)) digit = true;
            else special = true;
        }
        return upper && lower && digit && special;
    }

    bool isValidName(string n) {
        if (n.empty()) return false;
        for (char c : n)
            if (!isalpha(c) && c != ' ') return false;
        return true;
    }

    bool isValidPIN(int p) {
        return p >= 1000 && p <= 9999;
    }

    bool isValidAadhaar(string a) {
        return a.length() == 12;
    }

    bool isValidPAN(string p) {
        return p.length() == 10;
    }

    // OTP 
    int generateOTP() {
        return rand() % 9000 + 1000;
    }

    bool verifyOTP() {
        int otp = generateOTP();
        cout << "OTP sent: " << otp << endl;
        int entered;
        cout << "Enter OTP: ";
        cin >> entered;
        return otp == entered;
    }

    //  CREATE ACCOUNT 
    void createAccount() {
        cout << "\nEnter Account Number: ";
        cin >> accountNumber;

        cout << "Enter Name: ";
        cin.ignore();
        getline(cin, name);
        while (!isValidName(name)) {
            cout << "Invalid name! Enter again: ";
            getline(cin, name);
        }

        int t;
        cout << "Account Type (1-Savings, 2-Current): ";
        cin >> t;
        type = (t == 1) ? SAVINGS : CURRENT;

        cout << "Set Password: ";
        cin >> password;
        while (!isValidPassword(password)) {
            cout << "Weak password! Re-enter: ";
            cin >> password;
        }

        cout << "Set 4-digit PIN: ";
        cin >> pin;
        while (!isValidPIN(pin)) {
            cout << "Invalid PIN! Re-enter: ";
            cin >> pin;
        }

        cout << "Enter Aadhaar (12 digits): ";
        cin >> aadhaar;
        while (!isValidAadhaar(aadhaar)) {
            cout << "Invalid Aadhaar! Re-enter: ";
            cin >> aadhaar;
        }

        cout << "Enter PAN (10 characters): ";
        cin >> pan;
        while (!isValidPAN(pan)) {
            cout << "Invalid PAN! Re-enter: ";
            cin >> pan;
        }

        cout << "Security Question (favorite color): ";
        cin >> securityAnswer;

        cout << "Initial Deposit: ";
        cin >> balance;

        if (type == SAVINGS && balance < 500) {
            cout << "Minimum balance for Savings is ₹500\n";
            return;
        }
        if (type == CURRENT && balance < 5000) {
            cout << "Minimum balance for Current is ₹5000\n";
            return;
        }

        if (!verifyOTP()) {
            cout << "OTP verification failed. Account not created.\n";
            return;
        }

        cout << "Account created successfully!\n";
    }

    int getAccNo() { return accountNumber; }

    // LOGIN 
    bool login() {
        if (isLocked) {
            if (difftime(time(0), lockTime) < 30) {
                cout << "Account locked. Try later.\n";
                return false;
            } else {
                isLocked = false;
                attempts = 0;
            }
        }

        string pass;
        cout << "Enter Password: ";
        cin >> pass;

        if (pass != password) {
            attempts++;
            cout << "Wrong password!\n";
            if (attempts >= 3) {
                isLocked = true;
                lockTime = time(0);
                cout << "Account locked for 30 seconds.\n";
            }
            return false;
        }

        if (!verifyOTP()) {
            cout << "OTP verification failed.\n";
            return false;
        }

        cout << "Login successful!\n";
        return true;
    }

    //  PASSWORD MANAGEMENT 
    void changePassword() {
        string oldPass, newPass;
        cout << "Enter current password: ";
        cin >> oldPass;

        if (oldPass != password) {
            cout << "Incorrect password!\n";
            return;
        }

        cout << "Enter new password: ";
        cin >> newPass;
        if (!isValidPassword(newPass)) {
            cout << "Weak password!\n";
            return;
        }

        password = newPass;
        cout << "Password changed successfully!\n";
    }

    void forgotPassword() {
        string ans;
        cout << "Security Answer: ";
        cin >> ans;

        if (ans != securityAnswer) {
            cout << "Wrong answer!\n";
            return;
        }

        if (!verifyOTP()) {
            cout << "OTP failed!\n";
            return;
        }

        cout << "Enter new password: ";
        cin >> password;
        while (!isValidPassword(password)) {
            cout << "Weak password! Re-enter: ";
            cin >> password;
        }

        cout << "Password reset successful. Login again.\n";
    }

    //  PIN 
    bool verifyPIN() {
        int p;
        cout << "Enter PIN: ";
        cin >> p;
        return p == pin;
    }

    // BANKING 
    void checkBalance() {
        if (!verifyPIN()) {
            cout << "Wrong PIN!\n";
            return;
        }
        cout << "Balance: " << balance << endl;
    }

    void deposit() {
        double amt;
        cout << "Enter amount: ";
        cin >> amt;
        balance += amt;
        transactions.push_back("Deposited: " + to_string(amt));
    }

    void withdraw() {
        if (!verifyPIN()) {
            cout << "Wrong PIN!\n";
            return;
        }

        if (type == SAVINGS && withdrawalCount >= 5) {
            cout << "Savings withdrawal limit reached!\n";
            return;
        }

        double amt;
        cout << "Enter amount: ";
        cin >> amt;

        if (amt > balance) {
            cout << "Insufficient balance!\n";
            return;
        }

        balance -= amt;
        withdrawalCount++;
        transactions.push_back("Withdrawn: " + to_string(amt));
    }

    void showTransactions() {
        for (string t : transactions)
            cout << t << endl;
    }

    void checkInterest() {
        if (type == SAVINGS) {
            cout << "Annual Interest: " << balance * 0.04 << endl;
        } else {
            cout << "Interest not available for current account.\n";
        }
    }
};

// UNIQUE ACCOUNT CHECK
bool isAccountExists(vector<BankAccount>& accounts, int accNo) {
    for (auto &acc : accounts)
        if (acc.getAccNo() == accNo) return true;
    return false;
}

// MAIN
int main() {
    srand(time(0));
    vector<BankAccount> accounts;
    int choice;

    do {
        cout << "\n1.Create Account\n2.Login\n3.Forgot Password\n4.Exit\nChoice: ";
        cin >> choice;

        if (choice == 1) {
            BankAccount acc;
            acc.createAccount();
            if (!isAccountExists(accounts, acc.getAccNo()))
                accounts.push_back(acc);
            else
                cout << "Account number already exists!\n";
        }

        else if (choice == 2) {
            int accNo;
            cout << "Enter Account Number: ";
            cin >> accNo;

            for (auto &acc : accounts) {
                if (acc.getAccNo() == accNo && acc.login()) {
                    int opt;
                    do {
                        cout << "\n1.Balance 2.Deposit 3.Withdraw 4.History 5.Interest 6.Change Pass 7.Logout\n";
                        cin >> opt;
                        if (opt == 1) acc.checkBalance();
                        else if (opt == 2) acc.deposit();
                        else if (opt == 3) acc.withdraw();
                        else if (opt == 4) acc.showTransactions();
                        else if (opt == 5) acc.checkInterest();
                        else if (opt == 6) acc.changePassword();
                    } while (opt != 7);
                }
            }
        }

        else if (choice == 3) {
            int accNo;
            cout << "Enter Account Number: ";
            cin >> accNo;
            for (auto &acc : accounts)
                if (acc.getAccNo() == accNo)
                    acc.forgotPassword();
        }

    } while (choice != 4);

    return 0;
}