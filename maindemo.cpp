#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm> // Include the <algorithm> header for std::find_if

class BankAccount;

class Customer {
public:
    Customer(const std::string& name, const std::string& address, const std::string& contact)
        : name(name), address(address), contact(contact) {}

    std::string getName() const { return name; }
    std::string getAddress() const { return address; }
    std::string getContact() const { return contact; }

private:
    std::string name;
    std::string address;
    std::string contact;
};

class BankAccount {
public:
    BankAccount(long long accNum, const Customer& customer, double initialBalance)
        : accountNumber(accNum), customer(customer), balance(initialBalance) {}

    void deposit(double amount) {
        balance += amount;
    }

    bool withdraw(double amount) {
        if (balance >= amount) {
            balance -= amount;
            return true;
        } else {
            return false;
        }
    }

    double getBalance() const {
        return balance;
    }

    long long getAccountNumber() const {
        return accountNumber;
    }

    void display() const {
        std::cout << "Account Number: " << accountNumber << std::endl;
        std::cout << "Customer Name: " << customer.getName() << std::endl;
        std::cout << "Balance: " << balance << std::endl;
    }

    const Customer& getCustomer() const {
        return customer;
    }

    // Friend function declaration
    friend std::ostream& operator<<(std::ostream& os, const BankAccount& account);

private:
    long long accountNumber;
    Customer customer;
    double balance;
};

// Friend function definition
std::ostream& operator<<(std::ostream& os, const BankAccount& account) {
    os << "Account Number: " << account.getAccountNumber() << std::endl;
    os << "Customer Name: " << account.customer.getName() << std::endl;
    os << "Customer Address: " << account.customer.getAddress() << std::endl;
    os << "Customer Contact: " << account.customer.getContact() << std::endl;
    os << "Balance: " << account.balance << std::endl;
    return os;
}

class Bank {
public:
    Bank() {
        loadFromFile("bank_data.txt"); // Load previous data from the file
    }

    ~Bank() {
        saveToFile("bank_data.txt"); // Save data to the file before exiting
    }

    BankAccount* openAccount(long long accountNumber, const Customer& customer, double initialBalance) {
        // Validate the account number
        if (isValidAccountNumber(accountNumber)) {
            // Check if the customer exists, if not, add the customer
            auto it = std::find_if(customers.begin(), customers.end(),
                [&customer](const Customer& c) { return c.getName() == customer.getName(); });

            if (it == customers.end()) {
                std::string contact = customer.getContact();
                if (contact.size() == 10 && (contact[0] == '6' || contact[0] == '7' || contact[0] == '8' || contact[0] == '9')) { // Validate the customer contact
                    customers.push_back(customer);
                } else {
                    std::cout << "Error: Customer contact must be exactly 10 digits and start with 6, 7, 8, or 9." << std::endl;
                    return nullptr;
                }
            }

            BankAccount newAccount(accountNumber, customer, initialBalance);
            accounts.push_back(newAccount);
            return &accounts.back();
        } else {
            std::cout << "Error: Account number must have exactly 13 digits." << std::endl;
            return nullptr;
        }
    }

    BankAccount* findAccount(long long accountNumber) {
        for (auto& account : accounts) {
            if (account.getAccountNumber() == accountNumber) {
                return &account;
            }
        }
        return nullptr;
    }

    void saveToFile(const std::string& filename) {
        std::ofstream outputFile(filename);
        if (outputFile.is_open()) {
            for (const auto& account : accounts) {
                const Customer& customer = account.getCustomer();

                // Print Customer Details
                outputFile << "Customer Name: " << customer.getName() << std::endl;
                outputFile << "Customer Address: " << customer.getAddress() << std::endl;
                outputFile << "Customer Contact: " << customer.getContact() << std::endl;

                // Print Account Details
                outputFile << "Account Number: " << account.getAccountNumber() << std::endl;
                outputFile << "Balance: " << account.getBalance() << std::endl;

                outputFile << std::endl; // Add a blank line to separate each account
            }
            outputFile.close();
            std::cout << "Account details saved to " << filename << std::endl;
        } else {
            std::cout << "Error: Unable to open the file for saving account details." << std::endl;
        }
    }

private:
    std::vector<Customer> customers;
    std::vector<BankAccount> accounts;

    bool isValidAccountNumber(long long accountNumber) {
        // Convert the account number to a string and check its length
        std::string accNumStr = std::to_string(accountNumber);
        return accNumStr.length() == 13;
    }

    void loadFromFile(const std::string& filename) {
        std::ifstream inputFile(filename);
        if (inputFile.is_open()) {
            customers.clear();
            accounts.clear();

            std::string line;
            while (std::getline(inputFile, line)) {
                std::string name;
                std::string address;
                std::string contact;
                long long accountNumber;
                double balance;

                if (line.find("Customer Name: ") == 0) {
                    name = line.substr(15);
                    std::getline(inputFile, line);
                    address = line.substr(18);
                    std::getline(inputFile, line);
                    contact = line.substr(18);
                    std::getline(inputFile, line);
                    accountNumber = std::stoll(line.substr(16));
                    std::getline(inputFile, line);
                    balance = std::stod(line.substr(9));

                    customers.push_back(Customer(name, address, contact));
                    accounts.push_back(BankAccount(accountNumber, customers.back(), balance));

                    // Skip the blank line
                    std::getline(inputFile, line);
                }
            }

            inputFile.close();
            std::cout << "Loaded " << accounts.size() << " accounts from " << filename << std::endl;
        } else {
            std::cout << "No previous account data found. Starting with an empty bank." << std::endl;
        }
    }
};

int main() {
    Bank bank;

    int choice;
    long long accountNumber;
    double amount;
    std::string name, address, contact;
    std::string filename;

    while (true) {
        std::cout << "1. Open Account\n2. Perform Transactions\n3. Save Account Details\n4. Check Balance\n5. Exit" << std::endl;
        std::cin >> choice;

        switch (choice) {
            case 1: {
                std::cout << "Enter Customer Name: ";
                std::cin.ignore();
                std::getline(std::cin, name);
                std::cout << "Enter Customer Address: ";
                std::getline(std::cin, address);
                std::cout << "Enter Customer Contact: ";
                std::getline(std::cin, contact);
                std::cout << "Enter Account Number: ";
                std::cin >> accountNumber;
                std::cout << "Enter Initial Balance: ";
                std::cin >> amount;

                BankAccount* account = bank.openAccount(accountNumber, Customer(name, address, contact), amount);
                if (account) {
                    std::cout << "Account opened successfully." << std::endl;
                }
                break;
            }

            case 2: {
                std::cout << "Enter Account Number: ";
                std::cin >> accountNumber;
                BankAccount* accountToTransact = bank.findAccount(accountNumber);
                if (accountToTransact) {
                    std::cout << "1. Deposit\n2. Withdraw" << std::endl;
                    std::cin >> choice;
                    if (choice == 1) {
                        std::cout << "Enter Deposit Amount: ";
                        std::cin >> amount;
                        accountToTransact->deposit(amount);
                        std::cout << "Amount deposited successfully. Current Balance: " << accountToTransact->getBalance() << std::endl;
                    } else if (choice == 2) {
                        std::cout << "Enter Withdrawal Amount: ";
                        std::cin >> amount;
                        if (accountToTransact->withdraw(amount)) {
                            std::cout << "Amount withdrawn successfully. Current Balance: " << accountToTransact->getBalance() << std::endl;
                        } else {
                            std::cout << "Insufficient funds. Cannot withdraw." << std::endl;
                        }
                    } else {
                        std::cout << "Invalid transaction choice." << std::endl;
                    }
                } else {
                    std::cout << "Account not found." << std::endl;
                }
                break;
            }

            case 3: {
                 std::cout << "Enter the filename to save account details: ";
                std::cin.ignore();
                std::getline(std::cin, filename);
                bank.saveToFile(filename);
                break;
            }

            case 4: {
                std::cout << "Enter Account Number: ";
                std::cin >> accountNumber;
                BankAccount* account = bank.findAccount(accountNumber);
                if (account) {
                    std::cout << "Account Balance: " << account->getBalance() << std::endl;
                } else {
                    std::cout << "Account not found." << std::endl;
                }
                break;
            }

            case 5:
                std::cout << "Exiting..." << std::endl;
                return 0;

            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }
}
