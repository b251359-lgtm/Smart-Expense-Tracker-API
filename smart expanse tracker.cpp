#include <iostream>
#include <sqlite3.h>
#include <vector>
#include <iomanip>
#include <sstream>
using namespace std;

class ExpenseTracker {
private:
    sqlite3* DB;
    char* messageError;

public:
    ExpenseTracker() {
        int exit = sqlite3_open("expense_tracker.db", &DB);

        if (exit) {
            cerr << "Error opening database!" << endl;
        } else {
            cout << "Database opened successfully!" << endl;
        }

        createTable();
    }

    ~ExpenseTracker() {
        sqlite3_close(DB);
    }

    void createTable() {
        string sql = "CREATE TABLE IF NOT EXISTS TRANSACTIONS(" \
                     "ID INTEGER PRIMARY KEY AUTOINCREMENT, " \
                     "TYPE TEXT NOT NULL, " \
                     "CATEGORY TEXT NOT NULL, " \
                     "AMOUNT REAL NOT NULL, " \
                     "DESCRIPTION TEXT, " \
                     "MONTH TEXT NOT NULL);";

        int exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);

        if (exit != SQLITE_OK) {
            cerr << "Error Creating Table: " << messageError << endl;
            sqlite3_free(messageError);
        } else {
            cout << "Table ready!" << endl;
        }
    }

    void addTransaction() {
        string type, category, description, month;
        double amount;

        cin.ignore();

        cout << "\nEnter Type (Income/Expense): ";
        getline(cin, type);

        cout << "Enter Category: ";
        getline(cin, category);

        cout << "Enter Amount: ";
        cin >> amount;
        cin.ignore();

        cout << "Enter Description: ";
        getline(cin, description);

        cout << "Enter Month (e.g. May-2026): ";
        getline(cin, month);

        stringstream ss;
        ss << "INSERT INTO TRANSACTIONS(TYPE, CATEGORY, AMOUNT, DESCRIPTION, MONTH) VALUES('"
           << type << "','"
           << category << "',"
           << amount << ",'"
           << description << "','"
           << month << "');";

        string sql = ss.str();

        int exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);

        if (exit != SQLITE_OK) {
            cerr << "Error inserting record: " << messageError << endl;
            sqlite3_free(messageError);
        } else {
            cout << "Transaction added successfully!" << endl;
        }
    }

    static int displayCallback(void* NotUsed, int argc, char** argv, char** azColName) {
        for (int i = 0; i < argc; i++) {
            cout << setw(15) << azColName[i] << ": "
                 << (argv[i] ? argv[i] : "NULL") << endl;
        }
        cout << "-----------------------------------" << endl;
        return 0;
    }

    void viewTransactions() {
        string sql = "SELECT * FROM TRANSACTIONS;";

        int exit = sqlite3_exec(DB, sql.c_str(), displayCallback, 0, &messageError);

        if (exit != SQLITE_OK) {
            cerr << "Error fetching records: " << messageError << endl;
            sqlite3_free(messageError);
        }
    }

    void deleteTransaction() {
        int id;

        cout << "\nEnter Transaction ID to delete: ";
        cin >> id;

        stringstream ss;
        ss << "DELETE FROM TRANSACTIONS WHERE ID=" << id << ";";

        string sql = ss.str();

        int exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);

        if (exit != SQLITE_OK) {
            cerr << "Error deleting transaction: " << messageError << endl;
            sqlite3_free(messageError);
        } else {
            cout << "Transaction deleted successfully!" << endl;
        }
    }

    static int summaryCallback(void* data, int argc, char** argv, char** azColName) {
        cout << "\nMonthly Financial Summary" << endl;
        cout << "--------------------------" << endl;

        for (int i = 0; i < argc; i++) {
            cout << azColName[i] << ": " << (argv[i] ? argv[i] : "0") << endl;
        }

        return 0;
    }

    void monthlySummary() {
        string month;
        cin.ignore();

        cout << "\nEnter Month (e.g. May-2026): ";
        getline(cin, month);

        stringstream ss;

        ss << "SELECT "
           << "SUM(CASE WHEN TYPE='Income' THEN AMOUNT ELSE 0 END) AS Total_Income, "
           << "SUM(CASE WHEN TYPE='Expense' THEN AMOUNT ELSE 0 END) AS Total_Expense "
           << "FROM TRANSACTIONS WHERE MONTH='"
           << month << "';";

        string sql = ss.str();

        int exit = sqlite3_exec(DB, sql.c_str(), summaryCallback, 0, &messageError);

        if (exit != SQLITE_OK) {
            cerr << "Error generating summary: " << messageError << endl;
            sqlite3_free(messageError);
        }
    }
};

void menu() {
    cout << "\n========== SMART EXPENSE TRACKER ==========" << endl;
    cout << "1. Add Transaction" << endl;
    cout << "2. View Transactions" << endl;
    cout << "3. Delete Transaction" << endl;
    cout << "4. Monthly Summary" << endl;
    cout << "5. Exit" << endl;
    cout << "Choose an option: ";
}

int main() {
    ExpenseTracker tracker;

    int choice;

    do {
        menu();
        cin >> choice;

        switch (choice) {
            case 1:
                tracker.addTransaction();
                break;

            case 2:
                tracker.viewTransactions();
                break;

            case 3:
                tracker.deleteTransaction();
                break;

            case 4:
                tracker.monthlySummary();
                break;

            case 5:
                cout << "Exiting program..." << endl;
                break;

            default:
                cout << "Invalid choice! Try again." << endl;
        }

    } while (choice != 5);

    return 0;
}

