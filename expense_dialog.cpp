#include "expense_dialog.h"
#include "ui_expense_dialog.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDate>
#include <QPushButton>

ExpenseDialog::ExpenseDialog(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::ExpenseDialog)
{
    ui->setupUi(this);

    // Змінюємо назву самого вікна (на випадок, якщо рамка ОС відображається)
    this->setWindowTitle("Нова витрата");

    // Встановлюємо поточну дату та фокусуємося на сумі
    ui->dateEdit->setDate(QDate::currentDate());
    ui->dateEdit->setMinimumDate(QDate(2026, 1, 1));
    ui->dateEdit->setCalendarPopup(true);
    ui->editAmount->setFocus();

    // Зв'язуємо кнопки
    connect(ui->btnSave, &QPushButton::clicked, this, &ExpenseDialog::handleSave);
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::reject);

    // ================= ФІРМОВИЙ СТИЛЬ ДЛЯ ДІАЛОГОВОГО ВІКНА =================
    this->setStyleSheet(R"(
        /* Фон самого діалогового вікна */
        QDialog {
            background: qlineargradient(spread:pad, x1:0, y1:0, x2:0.8, y2:0.8,
                        stop:0 #1e293b, stop:0.5 #334155, stop:1 #1e293b);
            color: #ffffff;
            font-family: 'Segoe UI', sans-serif;
            font-size: 14px;
        }

        /* Головний заголовок вікна всередині форми (якщо ти додаси QLabel з назвою) */
        #labelTitle {
            color: #ffffff;
            font-size: 18px;
            font-weight: bold;
            padding-bottom: 10px;
            text-transform: none; /* Без капслоку для краси */
        }

        /* Текстові підписи полів (Сума, Категорія, Опис) */
        QLabel {
            color: #94a3b8; /* Приємний світло-сірий колір, який ідеально видно */
            font-weight: bold;
            text-transform: uppercase;
        }

        /* Поля введення (Сума, Опис, Вибір дати, Категорії) */
        QLineEdit, QComboBox, QDateEdit, QDoubleSpinBox {
            background: rgba(255, 255, 255, 0.1);
            color: white;
            border: 1px solid rgba(255, 255, 255, 0.2);
            border-radius: 10px;
            padding: 6px;
            selection-background-color: #6366f1;
        }

        /* Стан фокусу для полів введення */
        QLineEdit:focus, QComboBox:focus, QDateEdit:focus, QDoubleSpinBox:focus {
            border: 1px solid #6366f1;
            background: rgba(255, 255, 255, 0.15);
        }

        /* Випадаючий список категорій (внутрішні елементи) */
        QComboBox QAbstractItemView {
            background-color: #1e293b;
            color: white;
            selection-background-color: #6366f1;
            selection-color: white;
            border: 1px solid rgba(255, 255, 255, 0.1);
        }

        /* Кнопка "Зберегти" */
        #btnSave {
            background: #10b981;
            color: white;
            border: none;
            border-radius: 12px;
            font-weight: bold;
            padding: 8px 20px;
        }
        #btnSave:hover {
            background: #059669;
        }

        /* Кнопка "Скасувати" */
        #btnCancel {
            background: rgba(255, 255, 255, 0.08);
            color: #e2e8f0;
            border: 1px solid rgba(255, 255, 255, 0.1);
            border-radius: 12px;
            font-weight: bold;
            padding: 8px 20px;
        }
        #btnCancel:hover {
            background: rgba(255, 255, 255, 0.15);
        }
    )");
}

void ExpenseDialog::handleSave()
{
    if (ui->editAmount->value() <= 0) {
        QMessageBox::warning(this, "Помилка", "Введіть коректну суму.");
        return;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO expenses (date, category, amount, description) VALUES (?, ?, ?, ?)");
    query.addBindValue(ui->dateEdit->date().toString("yyyy-MM-dd"));
    query.addBindValue(ui->comboCategory->currentText());
    query.addBindValue(ui->editAmount->value());
    query.addBindValue(ui->editDesc->text());

    if (!query.exec()) {
        QMessageBox::critical(this, "Помилка БД", query.lastError().text());
        return;
    }

    accept();
}

ExpenseDialog::~ExpenseDialog()
{
    delete ui;
}
