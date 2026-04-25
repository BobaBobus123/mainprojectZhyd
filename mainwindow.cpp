#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "expense_dialog.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QHeaderView>
#include <QPropertyAnimation>
#include <QDate> // Додано для роботи з датами

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // 1. НАЛАШТУВАННЯ ПЕРІОДУ (Пункт: Початок 2026 - Сьогодні)
    QDate today = QDate::currentDate();
    ui->dateFrom->setDate(QDate(2026, 1, 1)); // Завжди 1 січня 2026
    ui->dateTo->setDate(today);               // Автоматично сьогодні (14.04, завтра буде 15.04 і т.д.)

    // Анімація появи вікна
    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(500);
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->start(QAbstractAnimation::DeleteWhenStopped);

    setupModel();

    // Конекти
    connect(ui->actionAdd, &QAction::triggered, this, &MainWindow::onAddExpense);
    connect(ui->actionDelete, &QAction::triggered, this, &MainWindow::onDeleteExpense);
    connect(ui->btnRefresh, &QPushButton::clicked, this, &MainWindow::updateStatistics);
    connect(ui->editSearch, &QLineEdit::textChanged, this, &MainWindow::onSearchChanged);

    // Оновлюємо дані відразу при запуску
    updateStatistics();
}

void MainWindow::setupModel() {
    model = new QSqlTableModel(this);
    model->setTable("expenses");
    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    model->select();

    model->setHeaderData(1, Qt::Horizontal, "Дата");
    model->setHeaderData(2, Qt::Horizontal, "Категорія");
    model->setHeaderData(3, Qt::Horizontal, "Сума (грн)");
    model->setHeaderData(4, Qt::Horizontal, "Опис");

    ui->tableView->setModel(model);
    ui->tableView->hideColumn(0);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setVisible(false);

    // Вмикаємо темне чергування рядків (стиль береться з UI)
    ui->tableView->setAlternatingRowColors(true);
}

void MainWindow::onSearchChanged(const QString &text) {
    if (!model) return;
    if (text.isEmpty()) {
        model->setFilter("");
    } else {
        model->setFilter(QString("category LIKE '%%1%' OR description LIKE '%%1%'").arg(text));
    }
    model->select();
}

void MainWindow::updateStatistics() {
    QPieSeries *series = new QPieSeries();

    // Фільтруємо запит до БД згідно з обраним періодом на екрані
    QString filter = QString("SELECT category, SUM(amount) FROM expenses "
                             "WHERE date >= '%1' AND date <= '%2' "
                             "GROUP BY category")
                         .arg(ui->dateFrom->date().toString("yyyy-MM-dd"))
                         .arg(ui->dateTo->date().toString("yyyy-MM-dd"));

    QSqlQuery query(filter);
    while (query.next()) {
        QPieSlice *slice = series->append(query.value(0).toString(), query.value(1).toDouble());
        slice->setLabelVisible(true);
        slice->setLabel(QString("%1: %2₴").arg(query.value(0).toString()).arg(query.value(1).toDouble()));
        slice->setLabelColor(Qt::white);
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("АНАЛІЗ ВИТРАТ");
    chart->setTitleBrush(QBrush(Qt::white));
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->setLabelColor(Qt::white);

    chart->setTheme(QChart::ChartThemeDark);
    chart->setBackgroundVisible(false);

    ui->chartView->setChart(chart);
    ui->chartView->setRenderHint(QPainter::Antialiasing);
}

void MainWindow::onAddExpense() {
    ExpenseDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        model->select();
        updateStatistics();
    }
}

void MainWindow::onDeleteExpense() {
    int row = ui->tableView->currentIndex().row();
    if (row < 0) return;
    if (QMessageBox::question(this, "Видалення", "Видалити вибраний запис?") == QMessageBox::Yes) {
        model->removeRow(row);
        model->select();
        updateStatistics();
    }
}

MainWindow::~MainWindow() {
    delete ui;
}
