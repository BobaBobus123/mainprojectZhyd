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

    // ГЛОБАЛЬНИЙ СТИЛЬ ПРОЄКТУ (ЄДИНА СТИЛІЗАЦІЯ)
    a.setStyleSheet(R"(
        /* Загальні налаштування вікон */
        QMainWindow, QDialog {
            background-color: #1b1b1b;
            color: #ffffff;
            font-family: 'Segoe UI', sans-serif;
            font-size: 14px;
        }

        QLabel {
            color: #ffffff;
        }

        /* Групуючі рамки */
        QGroupBox {
            color: #ffffff;
            font-size: 16px;
            font-weight: bold;
            border: 1px solid #353535;
            border-radius: 8px;
            margin-top: 12px;
            padding-top: 10px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 12px;
            padding: 0 6px;
        }

        /* Поля введення даних та вибору дат */
        QLineEdit, QComboBox, QDateEdit, QDoubleSpinBox {
            background-color: #2a2a2a;
            color: white;
            border: 1px solid #3d3d3d;
            border-radius: 6px;
            padding: 5px;
            selection-background-color: #3a86ff;
        }

        /* Кнопки */
        QPushButton {
            background-color: #3a86ff;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 6px 12px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #5390ff;
        }
        QPushButton:pressed {
            background-color: #2f6ed8;
        }

        /* Налаштування таблиці */
        QTableView {
            background-color: #222222;
            color: white;
            gridline-color: #333333;
            selection-background-color: #3a86ff;
            selection-color: white;
            alternate-background-color: #2a2a2a;
            border: 1px solid #3d3d3d;
            border-radius: 6px;
        }
        QHeaderView::section {
            background-color: #2a2a2a;
            color: white;
            padding: 6px;
            border: 1px solid #3d3d3d;
            font-weight: bold;
        }

        /* Стилізація вбудованих спливаючих календарів QCalendarWidget */
        QCalendarWidget QWidget {
            background-color: #1b1b1b;
            color: white;
        }
        QCalendarWidget QAbstractItemView:enabled {
            background-color: #2a2a2a;
            color: white;
            selection-background-color: #3a86ff;
            selection-color: white;
        }
        QCalendarWidget QNavigationWidget {
            background-color: #1b1b1b;
        }
        QCalendarWidget QMenu {
            background-color: #2a2a2a;
            color: white;
        }
    )");

    w.show();
    return a.exec();
}
