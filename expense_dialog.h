#ifndef EXPENSE_DIALOG_H
#define EXPENSE_DIALOG_H

#include <QDialog>

namespace Ui { class ExpenseDialog; }

class ExpenseDialog : public QDialog {
    Q_OBJECT

public:
    explicit ExpenseDialog(QWidget *parent = nullptr);
    ~ExpenseDialog();

private slots:
    void handleSave();

private:
    Ui::ExpenseDialog *ui;
};

#endif
