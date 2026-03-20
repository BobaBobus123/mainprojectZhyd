#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

class DatabaseManager {
public:
    static bool init() {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("expenses.db");
        if (!db.open()) return false;

        QSqlQuery query;
        return query.exec("CREATE TABLE IF NOT EXISTS expenses ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "date TEXT, "
                          "category TEXT, "
                          "amount REAL, "
                          "description TEXT)");
    }
};
#endif
