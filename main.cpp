#include "mainwindow.h"
#include "database_manager.h"
#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    if (!DatabaseManager::init()) {
        QMessageBox::critical(nullptr, "Помилка", "Не вдалося ініціалізувати базу даних.");
        return -1;
    }
    MainWindow w;
    w.show();
    return a.exec();
}
