#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include <QPushButton>

#include "wifi_lib.h"

#include <string>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setSignals(std::vector<Signals>* signal);
private:
    std::vector<Signals>* sig;
    Ui::MainWindow *ui;
    QtCharts::QBarSet* set;
    QtCharts::QBarSeries *series;
    QtCharts::QChart *chart;
    QStringList categories;
    QtCharts::QChartView *chartView;
    QtCharts::QBarCategoryAxis *axis;
    QPushButton *button;
    int checkMaxSignals(std::vector<Signals>* sig);
    void createChart();
private slots:
    void updateChart();

};
#endif // MAINWINDOW_H
