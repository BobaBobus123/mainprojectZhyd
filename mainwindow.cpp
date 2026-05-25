#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "expense_dialog.h"

#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QHeaderView>
#include <QDate>
#include <QBrush>
#include <QVector>

// Для підсвічування календаря
#include <QCalendarWidget>
#include <QTextCharFormat>

// Qt Charts
#include <QtCharts/QChart>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QDate today = QDate::currentDate();

    ui->dateFrom->setDate(QDate(2026, 1, 1));
    ui->dateTo->setDate(today);

    // Вмикаємо випадаючі календарі у полях вибору дат
    ui->dateFrom->setCalendarPopup(true);
    ui->dateTo->setCalendarPopup(true);

    // ================= UI ОБМЕЖЕННЯ РОЗМІРІВ =================
    ui->editSearch->setMaximumWidth(170);
    ui->editSearch->setFixedHeight(28);

    ui->btnRefresh->setMaximumWidth(110);
    ui->btnRefresh->setFixedHeight(28);

    ui->dateFrom->setMaximumWidth(130);
    ui->dateFrom->setFixedHeight(28);

    ui->dateTo->setMaximumWidth(130);
    ui->dateTo->setFixedHeight(28);

    // Ініціалізація бази даних та модели таблиці
    setupModel();

    // ================= SIGNALS & SLOTS =================
    connect(ui->actionAdd, &QAction::triggered, this, &MainWindow::onAddExpense);
    connect(ui->actionDelete, &QAction::triggered, this, &MainWindow::onDeleteExpense);

    connect(ui->btnRefresh, &QPushButton::clicked, this, &MainWindow::applyFilters);
    connect(ui->editSearch, &QLineEdit::textChanged, this, &MainWindow::applyFilters);

    connect(ui->dateFrom, &QDateEdit::dateChanged, this, &MainWindow::applyFilters);
    connect(ui->dateTo, &QDateEdit::dateChanged, this, &MainWindow::applyFilters);

    // Взаємне блокування дат
    connect(ui->dateFrom, &QDateEdit::dateChanged, this, &MainWindow::syncDateRange);
    connect(ui->dateTo, &QDateEdit::dateChanged, this, &MainWindow::syncDateRange);

    // Керування пропорціями: зменшуємо таблицю (коефіцієнт 2), збільшуємо графік (коефіцієнт 3)
    ui->splitter->setStretchFactor(0, 2);
    ui->splitter->setStretchFactor(1, 3);

    syncDateRange();
    applyFilters();
}

// ================= НАЛАШТУВАННЯ ТАБЛИЦІ =================
void MainWindow::setupModel()
{
    model = new QSqlTableModel(this);
    model->setTable("expenses");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();

    model->setHeaderData(1, Qt::Horizontal, "Дата");
    model->setHeaderData(2, Qt::Horizontal, "Категорія");
    model->setHeaderData(3, Qt::Horizontal, "Сума");
    model->setHeaderData(4, Qt::Horizontal, "Опис");

    ui->tableView->setModel(model);
    ui->tableView->hideColumn(0);

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setVisible(false);
}

// ================= ПІДЦВІЧУВАННЯ КАЛЕНДАРЯ =================
void MainWindow::refreshCalendarHighlights()
{
    QCalendarWidget *calFrom = ui->dateFrom->calendarWidget();
    QCalendarWidget *calTo = ui->dateTo->calendarWidget();

    if (!calFrom || !calTo) return;

    calFrom->setDateTextFormat(QDate(), QTextCharFormat());
    calTo->setDateTextFormat(QDate(), QTextCharFormat());

    QTextCharFormat highlightFormat;
    highlightFormat.setFontWeight(QFont::Bold);
    highlightFormat.setForeground(QBrush(QColor(255, 255, 255)));
    highlightFormat.setBackground(QBrush(QColor(99, 102, 241)));

    QSqlQuery query("SELECT DISTINCT date FROM expenses");
    while (query.next()) {
        QString dateStr = query.value(0).toString();
        QDate date = QDate::fromString(dateStr, "yyyy-MM-dd");
        if (date.isValid()) {
            calFrom->setDateTextFormat(date, highlightFormat);
            calTo->setDateTextFormat(date, highlightFormat);
        }
    }
}

// ================= ДОДАВАННЯ ВИТРАТИ =================
void MainWindow::onAddExpense()
{
    ExpenseDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        model->select();
        applyFilters();
    }
}

// ================= ВИДАЛЕННЯ ВИТРАТИ =================
void MainWindow::onDeleteExpense()
{
    int row = ui->tableView->currentIndex().row();
    if (row < 0) return;

    QMessageBox msg(this);
    msg.setWindowTitle("Підтвердження");
    msg.setText("Видалити цей запис?");
    msg.setInformativeText("Цю дію не можна скасувати");
    msg.setIcon(QMessageBox::Warning);
    msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

    if (msg.exec() != QMessageBox::Yes) return;

    model->removeRow(row);
    if (!model->submitAll()) {
        QMessageBox::critical(this, "Помилка видалення", model->lastError().text());
        return;
    }

    model->select();
    applyFilters();
}

// ================= ЗАСТОСУВАННЯ ФІЛЬТРІВ =================
void MainWindow::applyFilters()
{
    QString search = ui->editSearch->text();
    QString from = ui->dateFrom->date().toString("yyyy-MM-dd");
    QString to = ui->dateTo->date().toString("yyyy-MM-dd");

    QString filter = QString("date >= '%1' AND date <= '%2'").arg(from, to);
    if (!search.isEmpty()) {
        filter += QString(" AND (category LIKE '%%1%%' OR description LIKE '%%1%%')").arg(search);
    }

    model->setFilter(filter);
    model->select();

    updateStatistics();
    refreshCalendarHighlights();
}

// ================= АНАЛІЗ ВИТРАТ (ГРАФІК) =================
void MainWindow::updateStatistics()
{
    QPieSeries *series = new QPieSeries();

    QVector<QColor> colors = {
        QColor(99, 102, 241),
        QColor(16, 185, 129),
        QColor(245, 158, 11),
        QColor(239, 68, 68),
        QColor(6, 182, 212),
        QColor(236, 72, 153),
        QColor(168, 85, 247)
    };

    QSqlQuery query(QString(
                        "SELECT category, SUM(amount) "
                        "FROM expenses "
                        "WHERE date >= '%1' AND date <= '%2' "
                        "GROUP BY category"
                        ).arg(ui->dateFrom->date().toString("yyyy-MM-dd"),
                             ui->dateTo->date().toString("yyyy-MM-dd")));

    int colorIndex = 0;
    double totalAmount = 0;

    struct StatRow { QString category; double sum; };
    QVector<StatRow> rows;

    while (query.next()) {
        QString cat = query.value(0).toString();
        double sum = query.value(1).toDouble();
        totalAmount += sum;
        rows.append({cat, sum});
    }

    for (const auto& row : rows) {
        QPieSlice *slice = series->append(row.category, row.sum);

        double percentage = (totalAmount > 0) ? (row.sum / totalAmount * 100.0) : 0.0;

        slice->setLabel(QString("%1\n%2 грн (%3%)")
                            .arg(row.category)
                            .arg(row.sum, 0, 'f', 2)
                            .arg(percentage, 0, 'f', 1));

        slice->setLabelVisible(true);
        slice->setLabelPosition(QPieSlice::LabelOutside);

        slice->setLabelFont(QFont("Segoe UI", 10, QFont::Bold));
        slice->setLabelBrush(QBrush(Qt::white));

        QColor sliceColor = colors[colorIndex % colors.size()];
        slice->setBrush(QBrush(sliceColor));
        slice->setPen(QPen(QColor(30, 41, 59), 2));

        colorIndex++;
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("АНАЛІЗ ВИТРАТ");

    chart->setTitleFont(QFont("Segoe UI", 13, QFont::Bold));
    chart->setTitleBrush(QBrush(QColor(248, 250, 252)));
    chart->setBackgroundBrush(QBrush(QColor(30, 41, 59)));
    chart->setMargins(QMargins(10, 10, 10, 10));

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);
    chart->legend()->setFont(QFont("Segoe UI", 10, QFont::DemiBold));
    chart->legend()->setLabelColor(QColor(226, 232, 240));

    ui->chartView->setChart(chart);
    ui->chartView->setRenderHint(QPainter::Antialiasing);
}

// ================= СИНХРОНІЗАЦІЯ ДІАПАЗОНУ ДАТ =================
void MainWindow::syncDateRange()
{
    QDate from = ui->dateFrom->date();
    QDate to = ui->dateTo->date();

    ui->dateTo->setMinimumDate(from);
    ui->dateFrom->setMaximumDate(to);
}

MainWindow::~MainWindow()
{
    delete ui;
}
