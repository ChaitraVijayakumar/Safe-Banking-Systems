#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cctype>
#include <algorithm> 
#include <limits>
using namespace std;

enum AccountType { SAVINGS, CURRENT };
enum IDType {
    ID_AADHAAR,
    ID_PAN,
    ID_PASSPORT,
    ID_VOTER,
    ID_DL
};

class BankAccount {
private:
    int accountNumber;
    string name;
    string password;
    int pin;
    string securityAnswer;
    IDType idType;
    string idNumber;
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

    void selectIDProof() {
    int choice;
    bool valid = false;

    cout << "\nSelect ID Proof:\n";
    cout << "1.Aadhaar\n2.PAN\n3.Passport\n4.Voter ID\n5.Driving License\nChoice: ";
    cin >> choice;


    cout << "Enter ID Number: ";
    cin >> idNumber;

    switch (choice) {
        case 1:
            idType = ID_AADHAAR;
            valid = isValidAadhaar(idNumber);
            break;
        case 2:
            idType = ID_PAN;
            valid = isValidPAN(idNumber);
            break;
        case 3:
            idType = ID_PASSPORT;
            valid = isValidPassport(idNumber);
            break;
        case 4:
            idType = ID_VOTER;
            valid = isValidVoterID(idNumber);
            break;
        case 5:
            idType = ID_DL;
            valid = isValidDrivingLicense(idNumber);
            break;
        default:
            cout << "Invalid choice!\n";
            return;
    }

    while (!valid) {
        cout << "Invalid ID! Re-enter: ";
        cin >> idNumber;

        if (idType == ID_AADHAAR) valid = isValidAadhaar(idNumber);
        else if (idType == ID_PAN) valid = isValidPAN(idNumber);
        else if (idType == ID_PASSPORT) valid = isValidPassport(idNumber);
        else if (idType == ID_VOTER) valid = isValidVoterID(idNumber);
        else if (idType == ID_DL) valid = isValidDrivingLicense(idNumber);
    }
}

    bool isValidAadhaar(string a) {
    return a.length() == 12 && all_of(a.begin(), a.end(), ::isdigit);
}

bool isValidPAN(string p) {
    if (p.length() != 10) return false;
    return isupper(p[0]) && isupper(p[1]) && isupper(p[2]) &&
           isupper(p[3]) && isupper(p[4]) &&
           isdigit(p[5]) && isdigit(p[6]) && isdigit(p[7]) &&
           isdigit(p[8]) &&
           isupper(p[9]);
}

bool isValidPassport(string p) {
    return p.length() >= 8 && p.length() <= 9;
}

bool isValidVoterID(string v) {
    return v.length() == 10;
}

bool isValidDrivingLicense(string d) {
    return d.length() >= 10 && d.length() <= 16;
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

        //  SELECT ANY ONE ID PROOF
        selectIDProof();

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
