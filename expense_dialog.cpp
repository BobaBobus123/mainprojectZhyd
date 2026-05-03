#include "expense_dialog.h"
#include "ui_expense_dialog.h"
#include <QSqlQuery>
#include <QMessageBox>
#include <QDate>

ExpenseDialog::ExpenseDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::ExpenseDialog) {
    ui->setupUi(this);

    // Налаштування дат: Початок 2026 - Сьогодні (14.04.2026)
    ui->dateEdit->setDate(QDate::currentDate());
    ui->dateEdit->setMinimumDate(QDate(2026, 1, 1));

    ui->editAmount->setFocus();

    // Підключаємо кнопку вручну до handleSave
    connect(ui->btnSave, &QPushButton::clicked, this, &ExpenseDialog::handleSave);
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

void ExpenseDialog::handleSave() {
    if (ui->editAmount->value() <= 0) {
        QMessageBox::critical(this, "Помилка", "Сума повинна бути більшою за 0!");
        return;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO expenses (date, category, amount, description) VALUES (?, ?, ?, ?)");
    query.addBindValue(ui->dateEdit->date().toString("yyyy-MM-dd"));
    query.addBindValue(ui->comboCategory->currentText());
    query.addBindValue(ui->editAmount->value());
    query.addBindValue(ui->editDesc->text());

    if (query.exec()) {
        accept();
    } else {
        QMessageBox::critical(this, "Помилка БД", "Не вдалося зберегти запис!");
    }
}

ExpenseDialog::~ExpenseDialog() {
    delete ui;
}
