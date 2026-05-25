#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlTableModel>

// Qt Charts
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QChart>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddExpense();
    void onDeleteExpense();
    void applyFilters();
    void updateStatistics();
    void syncDateRange();

private:
    Ui::MainWindow *ui;
    QSqlTableModel *model;

    void setupModel();
    void refreshCalendarHighlights();
};

#endif // MAINWINDOW_H
