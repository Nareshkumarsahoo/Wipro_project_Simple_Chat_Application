#include <iostream>
#include <sqlite3.h>

int main() {

    sqlite3* db;

    int result = sqlite3_open("database/chat.db", &db);

    if (result != SQLITE_OK) {
        std::cout << "Database connection failed!" << std::endl;
        return 1;
    }

    std::cout << "Database connected successfully!" << std::endl;

    sqlite3_close(db);

    return 0;
}
