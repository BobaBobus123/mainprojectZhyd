#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlTableModel>

// Правильний спосіб підключення Charts у Qt 6
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QChart>

// Використовуємо макрос замість звичайного namespace
QT_USE_NAMESPACE

    QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddExpense();
    void onDeleteExpense();
    void updateStatistics();
    void onSearchChanged(const QString &text); // Новий слот для пошуку

private:
    Ui::MainWindow *ui;
    QSqlTableModel *model;
    void setupModel();
};

#endif // MAINWINDOW_H
